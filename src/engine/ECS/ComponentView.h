#pragma once

#include "ComponentIterator.h"

namespace Internal
{
	template<typename TWorld, typename TEntity, typename... Types>
	class ComponentViewTemplate
	{
	public:
		ComponentViewTemplate(const ComponentIteratorTemplate<TWorld, TEntity, Types...>& first, const ComponentIteratorTemplate<TWorld, TEntity, Types...>& last)
			: m_firstItr(first), m_lastItr(last)
		{
			if (m_firstItr.Get() == nullptr || (m_firstItr.Get()->IsPendingDestroy() && !m_firstItr.IncludePendingDestroy())
				|| !m_firstItr.Get()->template Has<Types...>())
			{
				++m_firstItr;
			}
		}

		const ComponentIteratorTemplate<TWorld, TEntity, Types...>& begin() const
		{
			return m_firstItr;
		}

		const ComponentIteratorTemplate<TWorld, TEntity, Types...>& end() const
		{
			return m_lastItr;
		}

	private:
		ComponentIteratorTemplate<TWorld, TEntity, Types...> m_firstItr;
		ComponentIteratorTemplate<TWorld, TEntity, Types...> m_lastItr;
	};
}
