#include "TypeSystem.h"
#include "Object.h"

GC_Object* RTTypes::CreateFromFile(World &world, ObjectType type)
{
	FromFileMap::const_iterator it = _ffm.find(type);
	if( _ffm.end() == it )
		return nullptr;
	return &it->second(world);
}

GC_Actor& RTTypes::CreateActor(World &world, ObjectType type, vec2d pos)
{
	assert(IsRegistered(type));
    assert(!GetTypeInfo(type).service);
	return GetTypeInfo(type).CreateActor(world, pos.x, pos.y);
}

GC_Service& RTTypes::CreateService(World &world, ObjectType type)
{
    assert(IsRegistered(type));
    assert(GetTypeInfo(type).service);
    return GetTypeInfo(type).CreateService(world);
}
