#pragma once

#include <unordered_map>
#include <functional>
#include <vector>
#include <algorithm>
#include <type_traits>

#include "TypeRegistry.h"
#include "Events.h"
#include "EntitySystem.h"
#include "EventSubscriber.h"

#include "ComponentContainer.h"

#include "ComponentIterator.h"
#include "ComponentView.h"

#include "EntityIterator.h"
#include "EntityView.h"

// Define ECS_TICK_NO_CLEANUP if you don't want the world to automatically cleanup dead entities
// at the beginning of each tick. This will require you to call cleanup() manually to prevent memory
// leaks.
//#define ECS_TICK_NO_CLEANUP


class World;
class Entity;

typedef std::allocator<Entity> Allocator;

namespace Internal
{
	using BaseComponentContainer = ComponentContainerInternalBase<World, Entity>;
	template<typename T>
	using ComponentContainer = ComponentContainerInternal<T, World, Entity>;

	template<typename... Types>
	using ComponentIterator = ComponentIteratorTemplate<World, Entity, Types...>;
	template<typename... Types>
	using ComponentView = ComponentViewTemplate<World, Entity, Types...>;

	using EntityIterator = EntityIteratorTemplate<World, Entity>;
	using EntityView = EntityViewTemplate<World, Entity>;
}

/**
 * The world creates, destroys, and manages entities. The lifetime of entities and _registered_ systems are handled by the world
 * (don't delete a system without unregistering it from the world first!), while event subscribers have their own lifetimes
 * (the world doesn't delete them automatically when the world is deleted).
 */
class World
{
public:
	using WorldAllocator = std::allocator_traits<Allocator>::rebind_alloc<World>;
	using EntityAllocator = std::allocator_traits<Allocator>::rebind_alloc<Entity>;
	using SystemAllocator = std::allocator_traits<Allocator>::rebind_alloc<EntitySystem>;
	using EntityPtrAllocator = std::allocator_traits<Allocator>::rebind_alloc<Entity*>;
	using SystemPtrAllocator = std::allocator_traits<Allocator>::rebind_alloc<EntitySystem*>;
	using SubscriberPtrAllocator = std::allocator_traits<Allocator>::rebind_alloc<Internal::BaseEventSubscriber*>;
	using SubscriberPairAllocator = std::allocator_traits<Allocator>::rebind_alloc<std::pair<const type_id_t, std::vector<Internal::BaseEventSubscriber*, SubscriberPtrAllocator>>>;

	/**
	 * Use this function to construct the world with a custom allocator.
	 */
	static World* createWorld(Allocator alloc)
	{
		WorldAllocator worldAlloc(alloc);
		World* world = std::allocator_traits<WorldAllocator>::allocate(worldAlloc, 1);
		std::allocator_traits<WorldAllocator>::construct(worldAlloc, world, alloc);

		return world;
	}

	/**
	 * Use this function to construct the world with the default allocator.
	 */
	static World* createWorld()
	{
		return createWorld(Allocator());
	}

	// Use this to destroy the world instead of calling delete.
	// This will emit OnEntityDestroyed events and call EntitySystem::unconfigure as appropriate.
	void destroyWorld()
	{
		WorldAllocator alloc(entAlloc);
		std::allocator_traits<WorldAllocator>::destroy(alloc, this);
		std::allocator_traits<WorldAllocator>::deallocate(alloc, this, 1);
	}

	World(Allocator alloc)
		: entAlloc(alloc)
		, systemAlloc(alloc)
		, entities({}, EntityPtrAllocator(alloc))
		, systems({}, SystemPtrAllocator(alloc))
		, subscribers({}, 0, std::hash<type_id_t>(), std::equal_to<type_id_t>(), SubscriberPtrAllocator(alloc))
	{
	}

	/**
	 * Destroying the world will emit OnEntityDestroyed events and call EntitySystem::unconfigure() as appropriate.
	 *
	 * Use World::destroyWorld to destroy and deallocate the world, do not manually delete the world!
	 */
	~World();

	/**
	 * Create a new entity. This will emit the OnEntityCreated event.
	 */
	Entity* create()
	{
		++lastEntityId;
		Entity* ent = std::allocator_traits<EntityAllocator>::allocate(entAlloc, 1);
		std::allocator_traits<EntityAllocator>::construct(entAlloc, ent, this, lastEntityId);
		entities.push_back(ent);

		emit<OnEntityCreated>({ ent });

		return ent;
	}

	/**
	 * Destroy an entity. This will emit the OnEntityDestroy event.
	 *
	 * If immediate is false (recommended), then the entity won't be immediately
	 * deleted but instead will be removed at the beginning of the next tick() or
	 * when cleanup() is called. OnEntityDestroyed will still be called immediately.
	 *
	 * This function is safe to call multiple times on a single entity. Note that calling
	 * this once with immediate = false and then calling it with immediate = true will
	 * remove the entity from the pending destroy queue and will immediately destroy it
	 * _without_ emitting a second OnEntityDestroyed event.
	 *
	 * A warning: Do not set immediate to true if you are currently iterating through entities!
	 */
	void destroy(Entity* ent, bool immediate = false);

	/**
	 * Delete all entities in the pending destroy queue. Returns true if any entities were cleaned up,
	 * false if there were no entities to clean up.
	 */
	bool cleanup();

	/**
	 * Reset the world, destroying all entities. Entity ids will be reset as well.
	 */
	void reset();

	/**
	 * Register a system. The world will manage the memory of the system unless you unregister the system.
	 */
	EntitySystem* registerSystem(EntitySystem* system)
	{
		systems.push_back(system);
		system->configure(this);

		return system;
	}

	/**
	 * Unregister a system.
	 */
	void unregisterSystem(EntitySystem* system)
	{
		systems.erase(std::remove(systems.begin(), systems.end(), system), systems.end());
		system->unconfigure(this);
	}

	void enableSystem(EntitySystem* system)
	{
		const auto it = std::find(disabledSystems.begin(), disabledSystems.end(), system);
		if (it != disabledSystems.end())
		{
			disabledSystems.erase(it);
			systems.push_back(system);
		}
	}

	void disableSystem(EntitySystem* system)
	{
		const auto it = std::find(systems.begin(), systems.end(), system);
		if (it != systems.end())
		{
			systems.erase(it);
			disabledSystems.push_back(system);
		}
	}

	/**
	 * Subscribe to an event.
	 */
	template<typename T>
	void subscribe(EventSubscriber<T>* subscriber)
	{
		auto index = getTypeIndex<T>();
		auto found = subscribers.find(index);
		if (found == subscribers.end())
		{
			std::vector<Internal::BaseEventSubscriber*, SubscriberPtrAllocator> subList(entAlloc);
			subList.push_back(subscriber);

			subscribers.insert({ index, subList });
		}
		else
		{
			found->second.push_back(subscriber);
		}
	}

	/**
	 * Unsubscribe from an event.
	 */
	template<typename T>
	void unsubscribe(EventSubscriber<T>* subscriber)
	{
		auto index = getTypeIndex<T>();
		auto found = subscribers.find(index);
		if (found != subscribers.end())
		{
			found->second.erase(std::remove(found->second.begin(), found->second.end(), subscriber), found->second.end());
			if (found->second.empty())
			{
				subscribers.erase(found);
			}
		}
	}

	/**
	 * Unsubscribe from all events. Don't be afraid of the void pointer, just pass in your subscriber as normal.
	 */
	void unsubscribeAll(void* subscriber)
	{
		for (auto kv : subscribers)
		{
			kv.second.erase(std::remove(kv.second.begin(), kv.second.end(), subscriber), kv.second.end());
			if (kv.second.empty())
			{
				subscribers.erase(kv.first);
			}
		}
	}

	/**
	 * Emit an event. This will do nothing if there are no subscribers for the event type.
	 */
	template<typename T>
	void emit(const T& event)
	{
		auto found = subscribers.find(getTypeIndex<T>());
		if (found != subscribers.end())
		{
			for (auto* base : found->second)
			{
				auto* sub = reinterpret_cast<EventSubscriber<T>*>(base);
				sub->receive(this, event);
			}
		}
	}

	/**
	 * Run a function on each entity with a specific set of components. This is useful for implementing an EntitySystem.
	 *
	 * If you want to include entities that are pending destruction, set includePendingDestroy to true.
	 */
	template<typename... Types>
	void each(typename std::common_type<std::function<void(Entity*, ComponentHandle<Types>...)>>::type viewFunc, bool bIncludePendingDestroy = false);

	/**
	 * Run a function on all entities.
	 */
	void all(std::function<void(Entity*)> viewFunc, bool bIncludePendingDestroy = false);

	/**
	 * Get a view for entities with a specific set of components. The list of entities is calculated on the fly, so this method itself
	 * has little overhead. This is mostly useful with a range based for loop.
	 */
	template<typename... Types>
	Internal::ComponentView<Types...> each(bool bIncludePendingDestroy = false)
	{
		Internal::ComponentIterator<Types...> first(this, 0, false, bIncludePendingDestroy);
		Internal::ComponentIterator<Types...> last(this, getCount(), true, bIncludePendingDestroy);
		return Internal::ComponentView<Types...>(first, last);
	}

	Internal::EntityView all(bool bIncludePendingDestroy = false);

	size_t getCount() const
	{
		return entities.size();
	}

	Entity* getByIndex(size_t idx)
	{
		if (idx >= getCount())
			return nullptr;

		return entities[idx];
	}

	/**
	 * Get an entity by an id. This is a slow process.
	 */
	Entity* getById(size_t id) const;

	/**
	 * Tick the world. See the definition for ECS_TICK_TYPE at the top of this file for more information on
	 * passing data through tick().
	 */
	void tick(float data)
	{
#ifndef ECS_TICK_NO_CLEANUP
		cleanup();
#endif
		for (auto* system : systems)
		{
			system->tick(this, data);
		}
	}

	EntityAllocator& getPrimaryAllocator()
	{
		return entAlloc;
	}

private:
	EntityAllocator entAlloc;
	SystemAllocator systemAlloc;

	std::vector<Entity*, EntityPtrAllocator> entities;
	std::vector<EntitySystem*, SystemPtrAllocator> systems;
	std::vector<EntitySystem*> disabledSystems;

	std::unordered_map<
		type_id_t
		, std::vector<Internal::BaseEventSubscriber*, SubscriberPtrAllocator>
		, std::hash<type_id_t>
		, std::equal_to<type_id_t>
		, SubscriberPairAllocator
	> subscribers;

	size_t lastEntityId = 0;
};

/**
 * A container for components. Entities do not have any logic of their own, except of that which to manage
 * components. Components themselves are generally structs that contain data with which EntitySystems can
 * act upon, but technically any data type may be used as a component, though only one of each data type
 * may be on a single Entity at a time.
 */
class Entity
{
public:
	friend class World;

	const static size_t InvalidEntityId = 0;

	// Do not create entities yourself, use World::create().
	Entity(World* world, size_t id)
		: world(world), id(id)
	{
	}

	// Do not delete entities yourself, use World::destroy().
	~Entity()
	{
		removeAll();
	}

	/**
	 * Get the world associated with this entity.
	 */
	World* getWorld() const
	{
		return world;
	}

	/**
	 * Does this entity have a component?
	 */
	template<typename T>
	bool has() const
	{
		auto index = getTypeIndex<T>();
		return components.find(index) != components.end();
	}

	/**
	 * Does this entity have this list of components? The order of components does not matter.
	 */
	template<typename T, typename V, typename... Types>
	bool has() const
	{
		return has<T>() && has<V, Types...>();
	}

	/**
	 * Assign a new component (or replace an old one). All components must have a default constructor, though they
	 * may have additional constructors. You may pass arguments to this function the same way you would to a constructor.
	 *
	 * It is recommended that components be simple types (not const, not references, not pointers). If you need to store
	 * any of the above, wrap it in a struct.
	 */
	template<typename T, typename... Args>
	ComponentHandle<T> assign(Args&&... args);

	/**
	 * Remove a component of a specific type. Returns whether a component was removed.
	 */
	template<typename T>
	bool remove()
	{
		auto found = components.find(getTypeIndex<T>());
		if (found != components.end())
		{
			found->second->removed(this);
			found->second->destroy(world);

			components.erase(found);

			return true;
		}

		return false;
	}

	/**
	 * Remove all components from this entity.
	 */
	void removeAll()
	{
		for (auto pair : components)
		{
			pair.second->removed(this);
			pair.second->destroy(world);
		}

		components.clear();
	}

	/**
	 * Get a component from this entity.
	 */
	template<typename T>
	ComponentHandle<T> get();

	/**
	 * Call a function with components from this entity as arguments. This will return true if this entity has
	 * all specified components attached, and false if otherwise.
	 */
	template<typename... Types>
	bool with(typename std::common_type<std::function<void(ComponentHandle<Types>...)>>::type view)
	{
		if (!has<Types...>())
			return false;

		view(get<Types>()...); // variadic template expansion is fun
		return true;
	}

	/**
	 * Get this entity's id. Entity ids aren't too useful at the moment, but can be used to tell the difference between entities when debugging.
	 */
	size_t getEntityId() const
	{
		return id;
	}

	bool isPendingDestroy() const
	{
		return bPendingDestroy;
	}

private:
	std::unordered_map<type_id_t, Internal::BaseComponentContainer*> components;
	World* world;

	size_t id;
	bool bPendingDestroy = false;
};

inline World::~World()
{
	for (auto* ent : entities)
	{
		if (!ent->isPendingDestroy())
		{
			ent->bPendingDestroy = true;
			emit<OnEntityDestroyed>({ ent });
		}

		std::allocator_traits<EntityAllocator>::destroy(entAlloc, ent);
		std::allocator_traits<EntityAllocator>::deallocate(entAlloc, ent, 1);
	}

	for (auto* system : systems)
	{
		system->unconfigure(this);
		std::allocator_traits<SystemAllocator>::destroy(systemAlloc, system);
		std::allocator_traits<SystemAllocator>::deallocate(systemAlloc, system, 1);
	}
}

inline void World::destroy(Entity* ent, bool immediate)
{
	if (ent == nullptr)
		return;

	if (ent->isPendingDestroy())
	{
		if (immediate)
		{
			entities.erase(std::remove(entities.begin(), entities.end(), ent), entities.end());
			std::allocator_traits<EntityAllocator>::destroy(entAlloc, ent);
			std::allocator_traits<EntityAllocator>::deallocate(entAlloc, ent, 1);
		}

		return;
	}

	ent->bPendingDestroy = true;

	emit<OnEntityDestroyed>({ ent });

	if (immediate)
	{
		entities.erase(std::remove(entities.begin(), entities.end(), ent), entities.end());
		std::allocator_traits<EntityAllocator>::destroy(entAlloc, ent);
		std::allocator_traits<EntityAllocator>::deallocate(entAlloc, ent, 1);
	}
}

inline bool World::cleanup()
{
	size_t count = 0;
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&, this](Entity* ent) {
		if (ent->isPendingDestroy())
		{
			std::allocator_traits<EntityAllocator>::destroy(entAlloc, ent);
			std::allocator_traits<EntityAllocator>::deallocate(entAlloc, ent, 1);
			++count;
			return true;
		}

		return false;
	}), entities.end());

	return count > 0;
}

inline void World::reset()
{
	for (auto* ent : entities)
	{
		if (!ent->isPendingDestroy())
		{
			ent->bPendingDestroy = true;
			emit<OnEntityDestroyed>({ ent });
		}
		std::allocator_traits<EntityAllocator>::destroy(entAlloc, ent);
		std::allocator_traits<EntityAllocator>::deallocate(entAlloc, ent, 1);
	}

	entities.clear();
	lastEntityId = 0;
}

inline void World::all(std::function<void(Entity*)> viewFunc, bool bIncludePendingDestroy)
{
	for (auto* ent : all(bIncludePendingDestroy))
	{
		viewFunc(ent);
	}
}

inline Internal::EntityView World::all(bool bIncludePendingDestroy)
{
	const Internal::EntityIterator first(this, 0, false, bIncludePendingDestroy);
	const Internal::EntityIterator last(this, getCount(), true, bIncludePendingDestroy);
	return { first, last };
}

inline Entity* World::getById(size_t id) const
{
	if (id == Entity::InvalidEntityId || id > lastEntityId)
		return nullptr;

	// We should likely store entities in a map of id -> entity so that this is faster.
	for (auto* ent : entities)
	{
		if (ent->getEntityId() == id)
			return ent;
	}

	return nullptr;
}

template<typename... Types>
void World::each(typename std::common_type<std::function<void(Entity*, ComponentHandle<Types>...)>>::type viewFunc, bool bIncludePendingDestroy)
{
	for (Entity* ent : each<Types...>(bIncludePendingDestroy))
	{
		viewFunc(ent, ent->get<Types>()...);
	}
}

template<typename T, typename... Args>
ComponentHandle<T> Entity::assign(Args&&... args)
{
	auto found = components.find(getTypeIndex<T>());
	if (found != components.end())
	{
		Internal::ComponentContainer<T>* container = reinterpret_cast<Internal::ComponentContainer<T>*>(found->second);
		container->data = T(args...);

		auto handle = ComponentHandle<T>(&container->data);
		world->emit<OnComponentAssigned<T>>({ this, handle });
		return handle;
	}

	using ComponentAllocator = std::allocator_traits<World::EntityAllocator>::rebind_alloc<Internal::ComponentContainer<T>>;

	ComponentAllocator alloc(world->getPrimaryAllocator());

	Internal::ComponentContainer<T>* container = std::allocator_traits<ComponentAllocator>::allocate(alloc, 1);
	std::allocator_traits<ComponentAllocator>::construct(alloc, container, T(args...));

	components.insert({ getTypeIndex<T>(), container });

	auto handle = ComponentHandle<T>(&container->data);
	world->emit<OnComponentAssigned<T>>({ this, handle });
	return handle;
}

template<typename T>
ComponentHandle<T> Entity::get()
{
	auto found = components.find(getTypeIndex<T>());
	if (found != components.end())
	{
		return ComponentHandle<T>(&reinterpret_cast<Internal::ComponentContainer<T>*>(found->second)->data);
	}

	return ComponentHandle<T>();
}
