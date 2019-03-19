#pragma once

#include "EntityIterator.h"

namespace Internal
{
	template<typename TWorld, typename TEntity>
	class EntityViewTemplate
	{
	public:
		EntityViewTemplate(const EntityIteratorTemplate<TWorld, TEntity>& first, const EntityIteratorTemplate<TWorld, TEntity>& last)
			: firstItr(first), lastItr(last)
		{
			if (firstItr.get() == nullptr || (firstItr.get()->isPendingDestroy() && !firstItr.includePendingDestroy()))
			{
				++firstItr;
			}
		}

		const EntityIteratorTemplate<TWorld, TEntity>& begin() const
		{
			return firstItr;
		}

		const EntityIteratorTemplate<TWorld, TEntity>& end() const
		{
			return lastItr;
		}

	private:
		EntityIteratorTemplate<TWorld, TEntity> firstItr;
		EntityIteratorTemplate<TWorld, TEntity> lastItr;
	};
}
