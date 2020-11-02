#pragma once

template<typename T>
class Component
{
public:
	Component()
		: m_component(nullptr)
	{
	}

	Component(T* component)
		: m_component(component)
	{
	}

	T* operator->() const
	{
		return m_component;
	}

	operator bool() const
	{
		return IsValid();
	}

	T& Get()
	{
		return *m_component;
	}

	bool IsValid() const
	{
		return m_component != nullptr;
	}

private:
	T* m_component;
};
