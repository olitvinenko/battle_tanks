#pragma once

typedef unsigned int TypeIndex;

class TypeRegistry
{
public:
	TypeRegistry();

	TypeIndex getIndex() const;

private:
	static TypeIndex nextIndex;
	TypeIndex index;
};

#define ECS_DECLARE_TYPE public: static TypeRegistry __ecs_type_reg
#define ECS_DEFINE_TYPE(name) TypeRegistry name::__ecs_type_reg

template<typename T>
TypeIndex getTypeIndex()
{
	return T::__ecs_type_reg.getIndex();
}