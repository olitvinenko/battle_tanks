#pragma once
#include "World.h"
#include "Object.h" // for ObjectType
#include "Serialization.h"
#include "math/Rect.h"
#include <string>
#include <map>
#include <set>
#include <vector>

class GC_Actor;
class GC_Service;

class RTTypes
{
public:
	struct EdItem
	{
		union
		{
			GC_Actor& (*CreateActor) (World &, float, float);
			GC_Service& (*CreateService) (World &);
		};
		union
		{
			GC_Actor& (*CreateDetachedActor)(Vector2 pos);
			GC_Service& (*CreateDetachedService)();
		};
		int           layer;
		float         align;
		float         offset;
		Vector2       size;
		const char*   name;
		const char*   desc;
		bool          service;
	};

private:
	typedef std::map<ObjectType, EdItem> type2item;
	typedef std::map<std::string, ObjectType> name2type;
	typedef std::vector<ObjectType> index2type;
	typedef std::map<ObjectType, GC_Object& (*) (World&)> FromFileMap;

	template<class T>
	static GC_Actor& DetachedActorCtor(Vector2 pos)
	{
		return *new T(pos);
	}

	template<class T>
	static GC_Service& DetachedServiceCtor()
	{
		return *new T();
	}

	template<class T>
	static GC_Actor& ActorCtor(World &world, float x, float y)
	{
		return world.New<T>(Vector2{ x, y });
	}

	template<class T>
	static GC_Service& ServiceCtor(World &world)
	{
		return world.New<T>();
	}

	template<class T>
	static GC_Object& FromFileCtor(World &world)
	{
		return world.New<T>(::FromFile());
	}

public:
	// access to singleton instance
	static RTTypes& Inst()
	{
		static RTTypes theInstance;
		return theInstance;
	}


	//
	// type registration
	//

	template<class T>
	void RegisterActor( const char *name, const char *desc, int layer, float width,
	                    float height, float align, float offset )
	{
		assert( !IsRegistered(T::GetTypeStatic()) );
		assert( 0 == _n2t.count(name) );
		EdItem ei = {};
		ei.desc = desc;  // index in localization table
		ei.name = name;
		ei.layer = layer;
		ei.size = { width, height };
		ei.align = align;
		ei.offset = offset;
		ei.service = false;
		ei.CreateActor = ActorCtor<T>;
		ei.CreateDetachedActor = DetachedActorCtor<T>;
		_t2i[T::GetTypeStatic()] = ei;
		_n2t[name] = T::GetTypeStatic();
		_i2t.push_back(T::GetTypeStatic());
	}
	template<class T>
	void RegisterService( const char *name, const char *desc )
	{
		assert( !IsRegistered(T::GetTypeStatic()) );
		assert( 0 == _n2t.count(name) );
		EdItem ei = {};
		ei.desc    = desc;
		ei.name    = name;
		ei.service = true;
		ei.CreateService  = ServiceCtor<T>;
		ei.CreateDetachedService = DetachedServiceCtor<T>;
		_t2i[T::GetTypeStatic()] = ei;
		_n2t[name] = T::GetTypeStatic();
		_i2t.push_back(T::GetTypeStatic());
	}

	template<class T>
	ObjectType RegType(const char *name)
	{
		// common
		assert(!_types.count(name));
		ObjectType type = (ObjectType) _types.size();
		_types.insert(name);
		// for serialization
		assert(!_ffm.count(type));
		_ffm[type] = FromFileCtor<T>;
		return type;
	}


	//
	// access type info
	//

	unsigned int GetTypeCount() const
	{
		return static_cast<unsigned int>(_i2t.size());
	}
	const EdItem& GetTypeInfoByIndex(unsigned int typeIndex) const
	{
		return _t2i.at(_i2t.at(typeIndex));
	}
	const EdItem& GetTypeInfo(ObjectType type) const
	{
		assert(IsRegistered(type));
		return _t2i.at(type);
	}
	ObjectType GetTypeByIndex(unsigned int typeIndex) const
	{
		return _i2t[typeIndex];
	}
	ObjectType GetTypeByName(const std::string &name) const
	{
		name2type::const_iterator it = _n2t.find(name);
		return _n2t.end() != it ? it->second : INVALID_OBJECT_TYPE;
	}
	bool IsRegistered(ObjectType type) const
	{
		return _t2i.find(type) != _t2i.end();
	}


	//
	// object creation
	//

    // for serialization
	GC_Object* CreateFromFile(World &world, ObjectType type);

    // for editor
	GC_Actor& CreateActor(World &world, ObjectType type, Vector2 pos);
    GC_Service& CreateService(World &world, ObjectType type);


private:
	// for editor
	type2item _t2i;
	name2type _n2t;
	index2type _i2t; // sort by desc
	// for serialization
	FromFileMap _ffm;
	// common
	std::set<std::string> _types;
	// use as singleton only
	RTTypes() {};
};
