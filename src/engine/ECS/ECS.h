#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <type_traits>

#include "TypeRegistry.h"
#include "Events.h"
#include "EntitySystem.h"
#include "EventListener.h"

#include "ComponentContainer.h"

#include "ComponentIterator.h"
#include "ComponentView.h"

#include "EntityIterator.h"
#include "EntityView.h"


class ECSWorld;
class Entity;

typedef std::allocator<Entity> Allocator;

namespace Internal
{
	using BaseComponentContainer = ComponentContainerInternalBase<ECSWorld, Entity>;
	template<typename T>
	using ComponentContainer = ComponentContainerInternal<T, ECSWorld, Entity>;

	template<typename... Types>
	using ComponentIterator = ComponentIteratorTemplate<ECSWorld, Entity, Types...>;
	template<typename... Types>
	using ComponentView = ComponentViewTemplate<ECSWorld, Entity, Types...>;

	using EntityIterator = EntityIteratorTemplate<ECSWorld, Entity>;
	using EntityView = EntityViewTemplate<ECSWorld, Entity>;
}


class ECSWorld
{
public:
	using WorldAllocator = std::allocator_traits<Allocator>::rebind_alloc<ECSWorld>;
	using EntityAllocator = std::allocator_traits<Allocator>::rebind_alloc<Entity>;
	using SystemAllocator = std::allocator_traits<Allocator>::rebind_alloc<EntitySystem>;
	using EntityPtrAllocator = std::allocator_traits<Allocator>::rebind_alloc<Entity*>;
	using SystemPtrAllocator = std::allocator_traits<Allocator>::rebind_alloc<EntitySystem*>;
	using ListenerPtrAllocator = std::allocator_traits<Allocator>::rebind_alloc<Internal::BaseEventListener*>;
	using SubscriberPairAllocator = std::allocator_traits<Allocator>::rebind_alloc<std::pair<const type_id_t, std::vector<Internal::BaseEventListener*, ListenerPtrAllocator>>>;

	static ECSWorld* CreateWorld(Allocator alloc)
	{
		WorldAllocator worldAlloc(alloc);
		ECSWorld* world = std::allocator_traits<WorldAllocator>::allocate(worldAlloc, 1);
		std::allocator_traits<WorldAllocator>::construct(worldAlloc, world, alloc);

		return world;
	}

	static ECSWorld* CreateWorld()
	{
		return CreateWorld(Allocator());
	}


	void DestroyWorld()
	{
		WorldAllocator alloc(m_entAlloc);
		std::allocator_traits<WorldAllocator>::destroy(alloc, this);
		std::allocator_traits<WorldAllocator>::deallocate(alloc, this, 1);
	}

	ECSWorld(Allocator alloc)
		: m_entAlloc(alloc)
		, m_systemAlloc(alloc)
		, m_entities({}, EntityPtrAllocator(alloc))
		, m_systems({}, SystemPtrAllocator(alloc))
		, m_subscribers({}, 0, std::hash<type_id_t>(), std::equal_to<type_id_t>(), ListenerPtrAllocator(alloc))
	{
	}

	~ECSWorld();

	Entity* Create()
	{
		++m_lastEntityId;
		Entity* ent = std::allocator_traits<EntityAllocator>::allocate(m_entAlloc, 1);
		std::allocator_traits<EntityAllocator>::construct(m_entAlloc, ent, this, m_lastEntityId);
		m_entities.push_back(ent);

		Emit<OnEntityCreated>({ ent });

		return ent;
	}

	void Destroy(Entity* ent, bool immediate = false);

	bool Cleanup();

	void Reset();

	EntitySystem* RegisterSystem(EntitySystem* system)
	{
		m_systems.push_back(system);
		system->Configure(this);

		return system;
	}

	void UnregisterSystem(EntitySystem* system)
	{
		m_systems.erase(std::remove(m_systems.begin(), m_systems.end(), system), m_systems.end());
		system->Unconfigure(this);
	}

	void EnableSystem(EntitySystem* system)
	{
		const auto it = std::find(m_disabledSystems.begin(), m_disabledSystems.end(), system);
		if (it != m_disabledSystems.end())
		{
			m_disabledSystems.erase(it);
			m_systems.push_back(system);
		}
	}

	void DisableSystem(EntitySystem* system)
	{
		const auto it = std::find(m_systems.begin(), m_systems.end(), system);
		if (it != m_systems.end())
		{
			m_systems.erase(it);
			m_disabledSystems.push_back(system);
		}
	}

	template<typename T>
	void Subscribe(EventListener<T>* subscriber)
	{
		auto index = GetTypeIndex<T>();
		auto found = m_subscribers.find(index);
		if (found == m_subscribers.end())
		{
			std::vector<Internal::BaseEventListener*, ListenerPtrAllocator> subList(m_entAlloc);
			subList.push_back(subscriber);

			m_subscribers.insert({ index, subList });
		}
		else
		{
			found->second.push_back(subscriber);
		}
	}

	template<typename T>
	void Unsubscribe(EventListener<T>* subscriber)
	{
		auto index = GetTypeIndex<T>();
		auto found = m_subscribers.find(index);
		if (found != m_subscribers.end())
		{
			found->second.erase(std::remove(found->second.begin(), found->second.end(), subscriber), found->second.end());
			if (found->second.empty())
			{
				m_subscribers.erase(found);
			}
		}
	}

	void UnsubscribeAll(void* subscriber)
	{
		for (auto kv : m_subscribers)
		{
			kv.second.erase(std::remove(kv.second.begin(), kv.second.end(), subscriber), kv.second.end());
			if (kv.second.empty())
			{
				m_subscribers.erase(kv.first);
			}
		}
	}

	template<typename T>
	void Emit(const T& event)
	{
		auto found = m_subscribers.find(GetTypeIndex<T>());
		if (found != m_subscribers.end())
		{
			for (auto* base : found->second)
			{
				auto* sub = reinterpret_cast<EventListener<T>*>(base);
				sub->Receive(this, event);
			}
		}
	}

	template<typename... Types>
	void Each(typename std::common_type<std::function<void(Entity*, Component<Types>...)>>::type viewFunc, bool bIncludePendingDestroy = false);

	void All(std::function<void(Entity*)> viewFunc, bool bIncludePendingDestroy = false);

	template<typename... Types>
	Internal::ComponentView<Types...> Each(bool bIncludePendingDestroy = false)
	{
		Internal::ComponentIterator<Types...> first(this, 0, false, bIncludePendingDestroy);
		Internal::ComponentIterator<Types...> last(this, GetCount(), true, bIncludePendingDestroy);
		return Internal::ComponentView<Types...>(first, last);
	}

	Internal::EntityView All(bool bIncludePendingDestroy = false);

	size_t GetCount() const
	{
		return m_entities.size();
	}

	Entity* GetByIndex(size_t idx)
	{
		if (idx >= GetCount())
			return nullptr;

		return m_entities[idx];
	}

	Entity* GetById(size_t id) const;

	void Tick(float data)
	{
#ifndef ECS_TICK_NO_CLEANUP
		Cleanup();
#endif
		for (auto* system : m_systems)
		{
			system->Tick(this, data);
		}
	}

	EntityAllocator& GetPrimaryAllocator()
	{
		return m_entAlloc;
	}

private:
	EntityAllocator m_entAlloc;
	SystemAllocator m_systemAlloc;

	std::vector<Entity*, EntityPtrAllocator> m_entities;
	std::vector<EntitySystem*, SystemPtrAllocator> m_systems;
	std::vector<EntitySystem*> m_disabledSystems;

	std::unordered_map<
		type_id_t
		, std::vector<Internal::BaseEventListener*, ListenerPtrAllocator>
		, std::hash<type_id_t>
		, std::equal_to<type_id_t>
		, SubscriberPairAllocator
	> m_subscribers;

	size_t m_lastEntityId = 0;
};

class Entity
{
public:
	friend class ECSWorld;

	const static size_t InvalidEntityId = 0;

	Entity(ECSWorld* world, size_t id)
		: m_world(world), m_id(id)
	{
	}

	~Entity()
	{
		RemoveAll();
	}

	ECSWorld* GetWorld() const
	{
		return m_world;
	}

	template<typename T>
	bool Has() const
	{
		auto index = GetTypeIndex<T>();
		return m_components.find(index) != m_components.end();
	}

	template<typename T, typename V, typename... Types>
	bool Has() const
	{
		return Has<T>() && Has<V, Types...>();
	}

	template<typename T, typename... Args>
	Component<T> Assign(Args&&... args);

	template<typename T>
	bool Remove()
	{
		auto found = m_components.find(GetTypeIndex<T>());
		if (found != m_components.end())
		{
			found->second->removed(this);
			found->second->destroy(m_world);

			m_components.erase(found);

			return true;
		}

		return false;
	}

	void RemoveAll()
	{
		for (auto pair : m_components)
		{
			pair.second->Removed(this);
			pair.second->Destroy(m_world);
		}

		m_components.clear();
	}

	template<typename T>
	Component<T> Get();

	template<typename... Types>
	bool With(typename std::common_type<std::function<void(Component<Types>...)>>::type view)
	{
		if (!Has<Types...>())
			return false;

		view(Get<Types>()...); // variadic template expansion is fun
		return true;
	}

	size_t GetEntityId() const
	{
		return m_id;
	}

	bool IsPendingDestroy() const
	{
		return m_pendingDestroy;
	}

private:
	std::unordered_map<type_id_t, Internal::BaseComponentContainer*> m_components;
	ECSWorld* m_world;

	size_t m_id;
	bool  m_pendingDestroy = false;
};

inline ECSWorld::~ECSWorld()
{
	for (auto* ent : m_entities)
	{
		if (!ent->IsPendingDestroy())
		{
			ent->m_pendingDestroy = true;
			Emit<OnEntityDestroyed>({ ent });
		}

		std::allocator_traits<EntityAllocator>::destroy(m_entAlloc, ent);
		std::allocator_traits<EntityAllocator>::deallocate(m_entAlloc, ent, 1);
	}

	for (auto* system : m_systems)
	{
		system->Unconfigure(this);
		std::allocator_traits<SystemAllocator>::destroy(m_systemAlloc, system);
		std::allocator_traits<SystemAllocator>::deallocate(m_systemAlloc, system, 1);
	}
}

inline void ECSWorld::Destroy(Entity* ent, bool immediate)
{
	if (ent == nullptr)
		return;

	if (ent->IsPendingDestroy())
	{
		if (immediate)
		{
			m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), ent), m_entities.end());
			std::allocator_traits<EntityAllocator>::destroy(m_entAlloc, ent);
			std::allocator_traits<EntityAllocator>::deallocate(m_entAlloc, ent, 1);
		}

		return;
	}

	ent->m_pendingDestroy = true;

	Emit<OnEntityDestroyed>({ ent });

	if (immediate)
	{
		m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), ent), m_entities.end());
		std::allocator_traits<EntityAllocator>::destroy(m_entAlloc, ent);
		std::allocator_traits<EntityAllocator>::deallocate(m_entAlloc, ent, 1);
	}
}

inline bool ECSWorld::Cleanup()
{
	size_t count = 0;
	m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [&, this](Entity* ent) {
		if (ent->IsPendingDestroy())
		{
			std::allocator_traits<EntityAllocator>::destroy(m_entAlloc, ent);
			std::allocator_traits<EntityAllocator>::deallocate(m_entAlloc, ent, 1);
			++count;
			return true;
		}

		return false;
	}), m_entities.end());

	return count > 0;
}

inline void ECSWorld::Reset()
{
	for (auto* ent : m_entities)
	{
		if (!ent->IsPendingDestroy())
		{
			ent->m_pendingDestroy = true;
			Emit<OnEntityDestroyed>({ ent });
		}
		std::allocator_traits<EntityAllocator>::destroy(m_entAlloc, ent);
		std::allocator_traits<EntityAllocator>::deallocate(m_entAlloc, ent, 1);
	}

	m_entities.clear();
	m_lastEntityId = 0;
}

inline void ECSWorld::All(std::function<void(Entity*)> viewFunc, bool bIncludePendingDestroy)
{
	for (auto* ent : All(bIncludePendingDestroy))
	{
		viewFunc(ent);
	}
}

inline Internal::EntityView ECSWorld::All(bool bIncludePendingDestroy)
{
	const Internal::EntityIterator first(this, 0, false, bIncludePendingDestroy);
	const Internal::EntityIterator last(this, GetCount(), true, bIncludePendingDestroy);
	return { first, last };
}

inline Entity* ECSWorld::GetById(size_t id) const
{
	if (id == Entity::InvalidEntityId || id > m_lastEntityId)
		return nullptr;

	for (auto* ent : m_entities)
	{
		if (ent->GetEntityId() == id)
			return ent;
	}

	return nullptr;
}

template<typename... Types>
void ECSWorld::Each(typename std::common_type<std::function<void(Entity*, Component<Types>...)>>::type viewFunc, bool bIncludePendingDestroy)
{
	for (Entity* ent : Each<Types...>(bIncludePendingDestroy))
	{
		viewFunc(ent, ent->Get<Types>()...);
	}
}

template<typename T, typename... Args>
Component<T> Entity::Assign(Args&&... args)
{
	auto found = m_components.find(GetTypeIndex<T>());
	if (found != m_components.end())
	{
		Internal::ComponentContainer<T>* container = reinterpret_cast<Internal::ComponentContainer<T>*>(found->second);
		container->data = T(args...);

		auto handle = Component<T>(&container->data);
		m_world->Emit<OnComponentAssigned<T>>({ this, handle });
		return handle;
	}

	using ComponentAllocator = std::allocator_traits<ECSWorld::EntityAllocator>::rebind_alloc<Internal::ComponentContainer<T>>;

	ComponentAllocator alloc(m_world->GetPrimaryAllocator());

	Internal::ComponentContainer<T>* container = std::allocator_traits<ComponentAllocator>::allocate(alloc, 1);
	std::allocator_traits<ComponentAllocator>::construct(alloc, container, T(args...));

	m_components.insert({ GetTypeIndex<T>(), container });

	auto handle = Component<T>(&container->data);
	m_world->Emit<OnComponentAssigned<T>>({ this, handle });
	return handle;
}

template<typename T>
Component<T> Entity::Get()
{
	auto found = m_components.find(GetTypeIndex<T>());
	if (found != m_components.end())
	{
		return Component<T>(&reinterpret_cast<Internal::ComponentContainer<T>*>(found->second)->data);
	}

	return Component<T>();
}
