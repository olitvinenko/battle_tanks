#pragma once

#include "Events.h"
#include <memory>

namespace Internal
{
	template<typename TWorld, typename TEntity>
	struct ComponentContainerInternalBase
	{
		virtual ~ComponentContainerInternalBase() = default;

		// This should only ever be called by the entity itself.
		virtual void Destroy(TWorld* world) = 0;

		// This will be called by the entity itself
		virtual void Removed(TEntity* ent) = 0;
	};

	template<typename TComponent, typename TWorld, typename TEntity>
	struct ComponentContainerInternal : ComponentContainerInternalBase<TWorld, TEntity>
	{
		ComponentContainerInternal() {}
		ComponentContainerInternal(const TComponent& data) : data(data) {}

		TComponent data;

	protected:
		void Destroy(TWorld* world) override
		{
			using ComponentAllocator = typename std::allocator_traits<typename TWorld::EntityAllocator>::template rebind_alloc<ComponentContainerInternal<TComponent, TWorld, TEntity>>;

			ComponentAllocator alloc(world->GetPrimaryAllocator());
			std::allocator_traits<ComponentAllocator>::destroy(alloc, this);
			std::allocator_traits<ComponentAllocator>::deallocate(alloc, this, 1);
		}

		void Removed(TEntity* ent) override
		{
			auto handle = Component<TComponent>(&data);
			ent->GetWorld()->template Emit<OnComponentRemoved<TComponent>>({ ent, handle });
		}
	};
}