#pragma once

namespace Internal
{
	template<typename TWorld, typename TEntity>
	class EntityIteratorTemplate
	{
	public:
		EntityIteratorTemplate(TWorld* world, size_t index, bool bIsEnd, bool bIncludePendingDestroy)
			: m_isEnd(bIsEnd), m_index(index), m_world(world), m_includePendingDestroy(bIncludePendingDestroy)
		{
			if (index >= world->GetCount())
				this->m_isEnd = true;
		}

		size_t GetIndex() const
		{
			return m_index;
		}

		bool IsEnd() const
		{
			return m_isEnd || m_index >= m_world->GetCount();
		}

		bool IncludePendingDestroy() const
		{
			return m_includePendingDestroy;
		}

		TWorld* GetWorld() const
		{
			return m_world;
		}

		TEntity* Get() const
		{
			if (IsEnd())
				return nullptr;

			return m_world->GetByIndex(m_index);
		}

		TEntity* operator*() const
		{
			return Get();
		}

		bool operator==(const EntityIteratorTemplate& other) const
		{
			if (m_world != other.m_world)
				return false;

			if (IsEnd())
				return other.IsEnd();

			return m_index == other.m_index;
		}

		bool operator!=(const EntityIteratorTemplate& other) const
		{
			if (m_world != other.m_world)
				return true;

			if (IsEnd())
				return !other.IsEnd();

			return m_index != other.m_index;
		}

		EntityIteratorTemplate& operator++()
		{
			++m_index;
			while (m_index < m_world->GetCount() && (Get() == nullptr || (Get()->IsPendingDestroy() && !m_includePendingDestroy)))
			{
				++m_index;
			}

			if (m_index >= m_world->GetCount())
				m_isEnd = true;

			return *this;
		}

	private:
		bool m_isEnd = false;
		size_t m_index;
		TWorld* m_world;
		bool m_includePendingDestroy;
	};
}