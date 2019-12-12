#pragma once

/**
	 * Think of this as a pointer to a component. Whenever you get a component from the world or an entity,
	 * it'll be wrapped in a ComponentHandle.
*/
template<typename T>
class ComponentHandle
{
public:
	ComponentHandle()
		: component(nullptr)
	{
	}

	ComponentHandle(T* component)
		: component(component)
	{
	}

	T* operator->() const
	{
		return component;
	}

	operator bool() const
	{
		return isValid();
	}

	T& get()
	{
		return *component;
	}

	bool isValid() const
	{
		return component != nullptr;
	}

private:
	T* component;
};
