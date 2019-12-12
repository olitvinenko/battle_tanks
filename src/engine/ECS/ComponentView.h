#pragma once

#include "ComponentIterator.h"

namespace Internal
{
	template<typename TWorld, typename TEntity, typename... Types>
	class ComponentViewTemplate
	{
	public:
		ComponentViewTemplate(const ComponentIteratorTemplate<TWorld, TEntity, Types...>& first, const ComponentIteratorTemplate<TWorld, TEntity, Types...>& last)
			: firstItr(first), lastItr(last)
		{
			if (firstItr.get() == nullptr || (firstItr.get()->isPendingDestroy() && !firstItr.includePendingDestroy())
				|| !firstItr.get()->template has<Types...>())
			{
				++firstItr;
			}
		}

		const ComponentIteratorTemplate<TWorld, TEntity, Types...>& begin() const
		{
			return firstItr;
		}

		const ComponentIteratorTemplate<TWorld, TEntity, Types...>& end() const
		{
			return lastItr;
		}

	private:
		ComponentIteratorTemplate<TWorld, TEntity, Types...> firstItr;
		ComponentIteratorTemplate<TWorld, TEntity, Types...> lastItr;
	};
}
