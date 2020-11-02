#pragma once

#include "TypeRegistry.h"
#include "Component.h"

class Entity;

struct OnEntityCreated
{
	ECS_DECLARE_TYPE;

	Entity* entity;
};

// Called when an entity is about to be destroyed.
struct OnEntityDestroyed
{
	ECS_DECLARE_TYPE;

	Entity* entity;
};

// Called when a component is assigned (not necessarily created).
template<typename T>
struct OnComponentAssigned
{
	ECS_DECLARE_TYPE;

	Entity* entity;
	Component<T> component;
};

// Called when a component is removed
template<typename T>
struct OnComponentRemoved
{
	ECS_DECLARE_TYPE;

	Entity* entity;
	Component<T> component;
};

template<typename T>
ECS_DEFINE_TYPE(OnComponentAssigned<T>);
template<typename T>
ECS_DEFINE_TYPE(OnComponentRemoved<T>);