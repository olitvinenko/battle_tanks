#pragma once

namespace Internal
{
	template<typename TWorld, typename TEntity, typename... Types>
	class ComponentIteratorTemplate
	{
	public:
		ComponentIteratorTemplate(TWorld* world, size_t index, bool bIsEnd, bool bIncludePendingDestroy)
			: bIsEnd(bIsEnd), index(index), world(world), bIncludePendingDestroy(bIncludePendingDestroy)
		{
			if (index >= world->getCount())
				this->bIsEnd = true;
		}

		size_t getIndex() const
		{
			return index;
		}

		bool isEnd() const
		{
			return bIsEnd || index >= world->getCount();
		}

		bool includePendingDestroy() const
		{
			return bIncludePendingDestroy;
		}

		TWorld* getWorld() const
		{
			return world;
		}

		TEntity* get() const
		{
			if (isEnd())
				return nullptr;

			return world->getByIndex(index);
		}

		TEntity* operator*() const
		{
			return get();
		}

		bool operator==(const ComponentIteratorTemplate<TWorld, TEntity, Types...>& other) const
		{
			if (world != other.world)
				return false;

			if (isEnd())
				return other.isEnd();

			return index == other.index;
		}

		bool operator!=(const ComponentIteratorTemplate<TWorld, TEntity, Types...>& other) const
		{
			if (world != other.world)
				return true;

			if (isEnd())
				return !other.isEnd();

			return index != other.index;
		}

		ComponentIteratorTemplate<TWorld, TEntity, Types...>& operator++()
		{
			++index;
			while (index < world->getCount() && (get() == nullptr || !get()->template has<Types...>() || (get()->isPendingDestroy() && !bIncludePendingDestroy)))
			{
				++index;
			}

			if (index >= world->getCount())
				bIsEnd = true;

			return *this;
		}

	private:
		bool bIsEnd = false;
		size_t index;
		TWorld* world;
		bool bIncludePendingDestroy;
	};
}