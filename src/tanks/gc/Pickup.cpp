#include "TypeReg.h"

#include "Pickup.h"
#include "Explosion.h"
#include "Indicators.h"
#include "Light.h"
#include "Macros.h"
#include "Particles.h"
#include "Player.h"
#include "Vehicle.h"
#include "Weapons.h"
#include "World.h"
#include "WorldEvents.h"
#include "SaveFile.h"
#include "mf/MapFile.h"


IMPLEMENT_2LIST_MEMBER(GC_Pickup, LIST_pickups, LIST_timestep);
IMPLEMENT_GRID_MEMBER(GC_Pickup, grid_pickup);

GC_Pickup::GC_Pickup(Vector2 pos)
  : GC_Actor(pos)
  , _timeAttached(0)
  , _timeRespawn(0)
{
	SetRespawn(false);
	SetBlinking(false);
	SetVisible(true);
}

GC_Pickup::GC_Pickup(FromFile)
  : GC_Actor(FromFile())
{
}

GC_Pickup::~GC_Pickup()
{
}

void GC_Pickup::Init(World &world)
{
	GC_Actor::Init(world);
	_label = &world.New<GC_HideLabel>(GetPos());
}

void GC_Pickup::Kill(World &world)
{
	if (GetAttached())
		Detach(world);
	SAFE_KILL(world, _label);
	GC_Actor::Kill(world);
}

void GC_Pickup::Serialize(World &world, SaveFile &f)
{
	GC_Actor::Serialize(world, f);

	f.Serialize(_timeAttached);
	f.Serialize(_timeRespawn);
	f.Serialize(_scriptOnPickup);
	f.Serialize(_label);
}

void GC_Pickup::Attach(World &world, GC_Vehicle &vehicle, bool asInitial)
{
	if (asInitial)
	{
		SAFE_KILL(world, _label);
	}

	assert(!GetAttached());
	_timeAttached = 0;
	SetFlags(GC_FLAG_PICKUP_ATTACHED, true);
	MoveTo(world, vehicle.GetPos());
	for( auto ls: world.eGC_Pickup._listeners )
		ls->OnAttach(*this, vehicle, asInitial);
	OnAttached(world, vehicle);
}

void GC_Pickup::Detach(World &world)
{
	assert(GetAttached());
	for( auto ls: world.eGC_Pickup._listeners )
		ls->OnDetach(*this);
	SetFlags(GC_FLAG_PICKUP_ATTACHED, false);
}

void GC_Pickup::Disappear(World &world)
{
	for( auto ls: world.eGC_Pickup._listeners )
		ls->OnDisappear(*this);

	if( GetAttached() )
		Detach(world);
	SetVisible(false);
	_timeAttached = 0;

	if (_label)
		MoveTo(world, _label->GetPos());
	else
		Kill(world);
}

void GC_Pickup::SetRespawnTime(float respawnTime)
{
	_timeRespawn = respawnTime;
}

float GC_Pickup::GetRespawnTime() const
{
	return _timeRespawn;
}

void GC_Pickup::SetBlinking(bool blink)
{
	SetFlags(GC_FLAG_PICKUP_BLINK, blink);
}

void GC_Pickup::TimeStep(World &world, float dt)
{
	_timeAttached += dt;

	if( !GetAttached() && !GetVisible() )
	{
		if( _timeAttached > _timeRespawn )  // FIXME
		{
			SetRespawn(false);
			SetVisible(true);
			for( auto ls: world.eGC_Pickup._listeners )
				ls->OnRespawn(*this);

			for( int n = 0; n < 50; ++n )
			{
				Vector2 a = Vec2dDirection(PI2 * (float) n / 50);
				world.New<GC_Particle>(GetPos() + a * 25, a * 25, PARTICLE_TYPE1, frand(0.5f) + 0.1f);
			}
		}
	}

	GC_Actor::TimeStep(world, dt);
}

void GC_Pickup::MapExchange(MapFile &f)
{
	GC_Actor::MapExchange(f);
	MAP_EXCHANGE_FLOAT(respawn_time,  _timeRespawn, GetDefaultRespawnTime());
	MAP_EXCHANGE_STRING(on_pickup, _scriptOnPickup, "");
}

PropertySet* GC_Pickup::NewPropertySet()
{
	return new MyPropertySet(this);
}

GC_Pickup::MyPropertySet::MyPropertySet(GC_Object *object)
  : BASE(object)
  , _propTimeRespawn(ObjectProperty::TYPE_INTEGER, "respawn_time")
  , _propOnPickup(ObjectProperty::TYPE_STRING, "on_pickup")
{
	_propTimeRespawn.SetIntRange(0, 1000000);
}

int GC_Pickup::MyPropertySet::GetCount() const
{
	return BASE::GetCount() + 2;
}

ObjectProperty* GC_Pickup::MyPropertySet::GetProperty(int index)
{
	if( index < BASE::GetCount() )
		return BASE::GetProperty(index);

	switch( index - BASE::GetCount() )
	{
	case 0: return &_propTimeRespawn; break;
	case 1: return &_propOnPickup;    break;
	}

	assert(false);
	return nullptr;
}

void GC_Pickup::MyPropertySet::MyExchange(World &world, bool applyToObject)
{
	BASE::MyExchange(world, applyToObject);

	GC_Pickup *obj = static_cast<GC_Pickup*>(GetObject());
	if( applyToObject )
	{
		obj->_timeRespawn = (float) _propTimeRespawn.GetIntValue() / 1000.0f;
		obj->_scriptOnPickup = _propOnPickup.GetStringValue();
	}
	else
	{
		_propTimeRespawn.SetIntValue(int(obj->_timeRespawn * 1000.0f + 0.5f));
		_propOnPickup.SetStringValue(obj->_scriptOnPickup);
	}
}

/////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_pu_Health)
{
	ED_ITEM( "pu_health", "obj_health", 4 /*layer*/ );
	return true;
}

GC_pu_Health::GC_pu_Health(Vector2 pos)
  : GC_Pickup(pos)
{
	SetRespawnTime( GetDefaultRespawnTime() );
}

GC_pu_Health::GC_pu_Health(FromFile)
  : GC_Pickup(FromFile())
{
}

AIPRIORITY GC_pu_Health::GetPriority(World &world, const GC_Vehicle &veh) const
{
	if( veh.GetHealth() < veh.GetHealthMax() )
		return AIP_HEALTH * (veh.GetHealth() / veh.GetHealthMax());

	return AIP_NOTREQUIRED;
}

void GC_pu_Health::OnAttached(World &world, GC_Vehicle &vehicle)
{
	vehicle.SetHealth(vehicle.GetHealthMax());
	Disappear(world);
}

/////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_pu_Mine)
{
	ED_ITEM( "pu_mine", "obj_mine", 5 /*layer*/ );
	return true;
}

GC_pu_Mine::GC_pu_Mine(Vector2 pos)
  : GC_Pickup(pos)
{
	SetRespawnTime(GetDefaultRespawnTime());
}

GC_pu_Mine::GC_pu_Mine(FromFile)
  : GC_Pickup(FromFile())
{
}

AIPRIORITY GC_pu_Mine::GetPriority(World &world, const GC_Vehicle &veh) const
{
	return AIP_NOTREQUIRED;
}

void GC_pu_Mine::OnAttached(World &world, GC_Vehicle &vehicle)
{
	world.New<GC_ExplosionStandard>(GetPos());
	Kill(world);
}

/////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_pu_Shield)
{
	ED_ITEM( "pu_shield", "obj_shield", 4 /*layer*/ );
	return true;
}

GC_pu_Shield::GC_pu_Shield(Vector2 pos)
  : GC_Pickup(pos)
  , _timeHit(0)
{
	SetRespawnTime( GetDefaultRespawnTime() );
}

GC_pu_Shield::GC_pu_Shield(FromFile)
  : GC_Pickup(FromFile())
{
}

GC_pu_Shield::~GC_pu_Shield()
{
}

AIPRIORITY GC_pu_Shield::GetPriority(World &world, const GC_Vehicle &veh) const
{
	return AIP_SHIELD;
}

void GC_pu_Shield::OnAttached(World &world, GC_Vehicle &vehicle)
{
	_vehicle = &vehicle;
	if (vehicle.GetShield())
		vehicle.GetShield()->Disappear(world);
	vehicle.SetShield(this);
}

void GC_pu_Shield::Detach(World &world)
{
	assert(_vehicle);
	_vehicle->SetShield(nullptr);
	SetBlinking(false);
	GC_Pickup::Detach(world);
}

void GC_pu_Shield::TimeStep(World &world, float dt)
{
	GC_Pickup::TimeStep(world, dt);

	if( GetAttached() )
	{
		if( GetTimeAttached() + 2.0f > PROTECT_TIME )
		{
			if( !GetBlinking() )
			{
				for( auto ls: world.eGC_pu_Shield._listeners )
					ls->OnExpiring(*this);
				SetBlinking(true);
			}
			if( GetTimeAttached() > PROTECT_TIME )
			{
				Disappear(world);
			}
		}
	}
}

void GC_pu_Shield::OnOwnerDamage(World &world, DamageDesc &dd)
{
	assert(_vehicle);
	if( dd.damage > 5 || 0 == rand() % 4 || world.GetTime() - _timeHit > 0.2f )
	{
		for( auto ls: world.eGC_pu_Shield._listeners )
			ls->OnOwnerDamage(*this, dd);

		const Vector2 &pos = _vehicle->GetPos();
		Vector2 dir = (dd.hit - pos).Normalize();
		Vector2 p = Vector2{ dir.y, -dir.x };
		Vector2 v = _vehicle->_lv;
		for( int i = 0; i < 7; i++ )
		{
			world.New<GC_Particle>(pos + dir * 26.0f + p * (float) (i<<1), v, PARTICLE_TYPE3, frand(0.4f)+0.1f);
			world.New<GC_Particle>(pos + dir * 26.0f - p * (float) (i<<1), v, PARTICLE_TYPE3, frand(0.4f)+0.1f);
		}
	}
	dd.damage *= 0.2f;
	_timeHit = world.GetTime();
}

void GC_pu_Shield::Serialize(World &world, SaveFile &f)
{
	GC_Pickup::Serialize(world, f);
	f.Serialize(_timeHit);
	f.Serialize(_vehicle);
}

///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_pu_Shock)
{
	ED_ITEM( "pu_shock", "obj_shock", 4 /*layer*/ );
	return true;
}

IMPLEMENT_1LIST_MEMBER(GC_pu_Shock, LIST_gsprites);

GC_pu_Shock::GC_pu_Shock(Vector2 pos)
  : GC_Pickup(pos)
  , _targetPos()
{
	SetRespawnTime(GetDefaultRespawnTime());
}

GC_pu_Shock::GC_pu_Shock(FromFile)
  : GC_Pickup(FromFile())
{
}

GC_pu_Shock::~GC_pu_Shock()
{
}

void GC_pu_Shock::Kill(World &world)
{
    SAFE_KILL(world, _light);
    GC_Pickup::Kill(world);
}

void GC_pu_Shock::Serialize(World &world, SaveFile &f)
{
	GC_Pickup::Serialize(world, f);
	f.Serialize(_light);
	f.Serialize(_vehicle);
	f.Serialize(_targetPos);
}

AIPRIORITY GC_pu_Shock::GetPriority(World &world, const GC_Vehicle &veh) const
{
	GC_Vehicle *tmp = FindNearVehicle(world, &veh);
	if( !tmp ) return AIP_NOTREQUIRED;

	if( tmp->GetOwner()->GetTeam() == veh.GetOwner()->GetTeam() && 0 != tmp->GetOwner()->GetTeam() )
	{
		return AIP_NOTREQUIRED;
	}

	return AIP_SHOCK;
}

void GC_pu_Shock::OnAttached(World &world, GC_Vehicle &vehicle)
{
	_vehicle = &vehicle;
}

void GC_pu_Shock::Detach(World &world)
{
	SetGridSet(true);
	SAFE_KILL(world, _light);
	_vehicle = nullptr;
	GC_Pickup::Detach(world);
}

GC_Vehicle* GC_pu_Shock::FindNearVehicle(World &world, const GC_RigidBodyStatic *ignore) const
{
	//
	// find the nearest enemy
	//

	float min_dist = AI_MAX_SIGHT * CELL_SIZE;
	float dist;

	GC_Vehicle *pNearTarget = nullptr;
	FOREACH( world.GetList(LIST_vehicles), GC_Vehicle, pTargetObj )
	{
		if( pTargetObj != ignore )
		{
			// distance to the object
			dist = (GetPos() - pTargetObj->GetPos()).len();

			if( dist < min_dist )
			{
				GC_RigidBodyStatic *pObstacle = world.TraceNearest(world.grid_rigid_s,
					_vehicle, GetPos(), pTargetObj->GetPos() - GetPos());

				if( pObstacle == pTargetObj )
				{
					pNearTarget = pTargetObj;
					min_dist = dist;
				}
			}
		}
	}

	return pNearTarget;
}

void GC_pu_Shock::TimeStep(World &world, float dt)
{
	GC_Pickup::TimeStep(world, dt);

	if( GetAttached() )
	{
		if (!_vehicle)
		{
			// vehicle was killed while attached
			Disappear(world);
		}
		else
		{
			if (GetGridSet())
			{
				if (GetTimeAttached() >= SHOCK_TIMEOUT)
				{
					if (GC_Vehicle *pNearTarget = FindNearVehicle(world, _vehicle))
					{
						SetGridSet(false);

						_targetPos = pNearTarget->GetPos();

						_light = &world.New<GC_Light>(GetPos(), GC_Light::LIGHT_DIRECT);
						_light->SetRadius(100);

						Vector2 tmp = _targetPos - GetPos();
						_light->SetLength(tmp.len());
						_light->SetLightDirection(tmp.Normalize());

						pNearTarget->TakeDamage(world, DamageDesc{ 1000, pNearTarget->GetPos(), _vehicle->GetOwner() });
					}
					else
					{
						_vehicle->TakeDamage(world, DamageDesc{ 1000, _vehicle->GetPos(), _vehicle->GetOwner() });
						Disappear(world);
					}
				}
			}
			else
			{
				float a = (GetTimeAttached() - SHOCK_TIMEOUT) * 5.0f;
				if (a > 1)
				{
					Disappear(world);
				}
				else
				{
					_light->SetIntensity(1.0f - powf(a, 6));
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

IMPLEMENT_SELF_REGISTRATION(GC_pu_Booster)
{
	ED_ITEM( "pu_booster", "obj_booster", 4 /*layer*/ );
	return true;
}

GC_pu_Booster::GC_pu_Booster(Vector2 pos)
  : GC_Pickup(pos)
{
	SetRespawnTime(GetDefaultRespawnTime());
}

GC_pu_Booster::GC_pu_Booster(FromFile)
  : GC_Pickup(FromFile())
{
}

GC_pu_Booster::~GC_pu_Booster()
{
}

void GC_pu_Booster::Serialize(World &world, SaveFile &f)
{
	GC_Pickup::Serialize(world, f);
	f.Serialize(_weapon);
}

AIPRIORITY GC_pu_Booster::GetPriority(World &world, const GC_Vehicle &veh) const
{
	if( !veh.GetWeapon() )
	{
		return AIP_NOTREQUIRED;
	}

	return veh.GetWeapon()->GetBooster() ? AIP_BOOSTER_HAVE : AIP_BOOSTER;
}

void GC_pu_Booster::OnAttached(World &world, GC_Vehicle &vehicle)
{
	if( GC_Weapon *w = vehicle.GetWeapon() )
	{
		if( w->GetBooster() )
			w->GetBooster()->Disappear(world);

		_weapon = w;
		_weapon->SetBooster(world, this);
	}
	else
	{
		// disappear if actor is not a weapon.
		Disappear(world);
	}
}

void GC_pu_Booster::Detach(World &world)
{
	if (_weapon)
	{
		_weapon->SetBooster(world, nullptr);
		_weapon = nullptr;
	}
	GC_Pickup::Detach(world);
}

void GC_pu_Booster::TimeStep(World &world, float dt)
{
	GC_Pickup::TimeStep(world, dt);
	if( GetAttached() )
	{
		if( GetTimeAttached() > BOOSTER_TIME )
		{
			Disappear(world);
		}
	}
}
