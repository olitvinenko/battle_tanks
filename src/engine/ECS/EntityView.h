#pragma once

#include "EntityIterator.h"

namespace Internal
{
	template<typename TWorld, typename TEntity>
	class EntityViewTemplate
	{
	public:
		EntityViewTemplate(const EntityIteratorTemplate<TWorld, TEntity>& first, const EntityIteratorTemplate<TWorld, TEntity>& last)
			: m_firstItr(first), m_lastItr(last)
		{
			if (m_firstItr.Get() == nullptr || (m_firstItr.Get()->IsPendingDestroy() && !m_firstItr.IncludePendingDestroy()))
			{
				++m_firstItr;
			}
		}

		const EntityIteratorTemplate<TWorld, TEntity>& begin() const
		{
			return m_firstItr;
		}

		const EntityIteratorTemplate<TWorld, TEntity>& end() const
		{
			return m_lastItr;
		}

	private:
		EntityIteratorTemplate<TWorld, TEntity> m_firstItr;
		EntityIteratorTemplate<TWorld, TEntity> m_lastItr;
	};
}
