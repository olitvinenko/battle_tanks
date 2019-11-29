#include "TypeReg.h"
#include "Light.h"
#include "Macros.h"
#include "World.h"
#include "WorldCfg.h"
#include "SaveFile.h"
#include "mf/MapFile.h"
#include <cfloat>

IMPLEMENT_SELF_REGISTRATION(GC_Light)
{
	return true;
}

IMPLEMENT_1LIST_MEMBER(GC_Light, LIST_lights);

GC_Light::GC_Light(vec2d pos, enumLightType type)
	: GC_Actor(pos)
	, _startTime(-FLT_MAX)
	, _timeout(0)
	, _aspect(1)
	, _offset(0)
	, _radius(200)
	, _intensity(1)
	, _type(type)
	, _lightDirection{ 1, 0 }
{
	SetActive(true);
}

GC_Light::GC_Light(FromFile)
	: GC_Actor(FromFile())
{
}

GC_Light::~GC_Light()
{
}

void GC_Light::Serialize(World &world, SaveFile &f)
{
	GC_Actor::Serialize(world, f);

	f.Serialize(_lightDirection);
	f.Serialize(_aspect);
	f.Serialize(_intensity);
	f.Serialize(_offset);
	f.Serialize(_radius);
	f.Serialize(_timeout);
	f.Serialize(_startTime);
	f.Serialize(_type);
}

void GC_Light::SetTimeout(World &world, float t)
{
	assert(t >= 0);
	assert(!CheckFlags(GC_FLAG_LIGHT_FADE));
	SetFlags(GC_FLAG_LIGHT_FADE, true);
	_startTime = world.GetTime();
	_timeout = t;
	world.Timeout(*this, t);
}

void GC_Light::Resume(World &world)
{
	Kill(world);
}

void GC_Light::SetActive(bool activate)
{
	SetFlags(GC_FLAG_LIGHT_ACTIVE, activate);
}

/////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_Spotlight)
{
	ED_ITEM( "spotlight", "obj_spotlight", 3 /*layer*/ );
	return true;
}

GC_Spotlight::GC_Spotlight(vec2d pos)
  : GC_Actor(pos)
{
	SetFlags(GC_FLAG_SPOTLIGHT_ACTIVE, true);
}

GC_Spotlight::GC_Spotlight(FromFile)
  : GC_Actor(FromFile())
{
}

GC_Spotlight::~GC_Spotlight()
{
}

void GC_Spotlight::Init(World &world)
{
	GC_Actor::Init(world);
	_light = &world.New<GC_Light>(GetPos() + GetDirection() * 7, GC_Light::LIGHT_SPOT);
	_light->SetActive(CheckFlags(GC_FLAG_SPOTLIGHT_ACTIVE));
	_light->SetRadius(200);
	_light->SetIntensity(1.0f);
	_light->SetOffset(170);
	_light->SetAspect(0.5f);
	_light->SetLightDirection(GetDirection());
}

void GC_Spotlight::Kill(World &world)
{
	SAFE_KILL(world, _light);
    GC_Actor::Kill(world);
}

void GC_Spotlight::Serialize(World &world, SaveFile &f)
{
	GC_Actor::Serialize(world, f);
	f.Serialize(_light);
}

void GC_Spotlight::MoveTo(World &world, const vec2d &pos)
{
	_light->MoveTo(world, pos + GetDirection() * 7);
	GC_Actor::MoveTo(world, pos);
}

void GC_Spotlight::MapExchange(MapFile &f)
{
	GC_Actor::MapExchange(f);

	float dir = GetDirection().Angle();
	int active = CheckFlags(GC_FLAG_SPOTLIGHT_ACTIVE) ? 1 : 0;

	MAP_EXCHANGE_INT(active, active, 1);
	MAP_EXCHANGE_FLOAT(dir, dir, 0);

	if( f.loading() )
	{
		SetDirection(Vec2dDirection(dir));
		SetFlags(GC_FLAG_SPOTLIGHT_ACTIVE, !!active);
	}
}

PropertySet* GC_Spotlight::NewPropertySet()
{
	return new MyPropertySet(this);
}

GC_Spotlight::MyPropertySet::MyPropertySet(GC_Object *object)
  : BASE(object)
  , _propActive( ObjectProperty::TYPE_INTEGER, "active"  )
  , _propDir( ObjectProperty::TYPE_FLOAT, "dir" )
{
	_propActive.SetIntRange(0, 1);
	_propDir.SetFloatRange(0, PI2);
}

int GC_Spotlight::MyPropertySet::GetCount() const
{
	return BASE::GetCount() + 2;
}

ObjectProperty* GC_Spotlight::MyPropertySet::GetProperty(int index)
{
	if( index < BASE::GetCount() )
		return BASE::GetProperty(index);

	switch( index - BASE::GetCount() )
	{
	case 0: return &_propActive;
    case 1: return &_propDir;
	}

	assert(false);
	return nullptr;
}

void GC_Spotlight::MyPropertySet::MyExchange(World &world, bool applyToObject)
{
	BASE::MyExchange(world, applyToObject);

	GC_Spotlight *tmp = static_cast<GC_Spotlight *>(GetObject());

	if( applyToObject )
	{
		tmp->_light->SetActive(0 != _propActive.GetIntValue());
		tmp->SetFlags(GC_FLAG_SPOTLIGHT_ACTIVE, 0 != _propActive.GetIntValue());
		tmp->SetDirection(Vec2dDirection(_propDir.GetFloatValue()));
		tmp->_light->SetLightDirection(tmp->GetDirection());
		tmp->_light->MoveTo(world, tmp->GetPos() + tmp->GetDirection() * 7);
	}
	else
	{
		_propActive.SetIntValue(tmp->_light->GetActive() ? 1 : 0);
		_propDir.SetFloatValue(tmp->GetDirection().Angle());
	}
}
