#pragma once

#include "Actor.h"
#include "memory/ObjectPtr.h"
#include "Rotator.h"

typedef float AIPRIORITY;

// ai priorities settings

// if the priority <= AIP_NOTREQUIRED than ai ignores the item
#define AIP_NOTREQUIRED     0.0f

// priority unit
#define AIP_NORMAL          1.0f

// the priority rate falls with the distance: p = (base - AIP_NORMAL * l / AI_MAX_DEPTH)
// where base - basic priority level, l - distance in cells


#define AIP_WEAPON_NORMAL   (AIP_NORMAL)        // weapon priority when no weapon at all
#define AIP_WEAPON_FAVORITE (AIP_NORMAL / 2)    // the priority of a favorite weapon
#define AIP_WEAPON_ADVANCED (AIP_NORMAL / 2)    // the priority of a weapon with the booster attached
#define AIP_HEALTH          (AIP_NORMAL)        // the priority of the 'health' item when the ai is almost dead
#define AIP_BOOSTER         (AIP_NORMAL)        // the priority of a booster
#define AIP_BOOSTER_HAVE    (AIP_BOOSTER / 10)  // the priority of a booster if we already have a booster attached
#define AIP_SHOCK           (AIP_NORMAL)        // the priority of a shock item
#define AIP_SHIELD          (AIP_NORMAL)        // the priority of a shield item

// TODO: move to ai
#define AI_MAX_SIGHT   20.0f
#define AI_MAX_DEPTH   50.0f


///////////////////////////////////////////////////////////////////////////////
// forward declarations

class GC_HideLabel;
class GC_Vehicle;
class GC_Light;
class GC_Crosshair;
class GC_Weapon;
class GC_RigidBodyStatic;

///////////////////////////////////////////////////////////////////////////////

#define GC_FLAG_PICKUP_BLINK             (GC_FLAG_ACTOR_ << 0)
#define GC_FLAG_PICKUP_AUTO              (GC_FLAG_ACTOR_ << 1)
#define GC_FLAG_PICKUP_RESPAWN           (GC_FLAG_ACTOR_ << 2)
#define GC_FLAG_PICKUP_VISIBLE           (GC_FLAG_ACTOR_ << 3)
#define GC_FLAG_PICKUP_ATTACHED          (GC_FLAG_ACTOR_ << 4)
#define GC_FLAG_PICKUP_                  (GC_FLAG_ACTOR_ << 5)

class GC_Pickup : public GC_Actor
{
	DECLARE_LIST_MEMBER(override);
	DECLARE_GRID_MEMBER();
    typedef GC_Actor base;

public:
	explicit GC_Pickup(Vector2 pos);
	explicit GC_Pickup(FromFile);
	virtual ~GC_Pickup();

	bool IsInitial() const { return !_label; } // initials do not have the label to respawn at

	void Attach(World &world, GC_Vehicle &vehicle, bool asInitial);

	const std::string& GetOnPickup() const { return _scriptOnPickup; }

	bool GetAttached() const { return CheckFlags(GC_FLAG_PICKUP_ATTACHED); }
	float GetRadius() const { return 8.0f; }

	float GetRespawnTime() const;
	void  SetRespawnTime(float respawnTime);

	bool GetRespawn() const       { return CheckFlags(GC_FLAG_PICKUP_RESPAWN); }
	void SetRespawn(bool respawn) { SetFlags(GC_FLAG_PICKUP_RESPAWN, respawn); }

	void SetVisible(bool bShow) { SetFlags(GC_FLAG_PICKUP_VISIBLE, bShow); }
	bool GetVisible() const { return CheckFlags(GC_FLAG_PICKUP_VISIBLE); }

	float GetTimeAttached() const { assert(GetAttached()); return _timeAttached; }

	void SetBlinking(bool blink);
	bool GetBlinking() const { return CheckFlags(GC_FLAG_PICKUP_BLINK); }

	// if 0 then item considered useless and will not be taken
	virtual AIPRIORITY GetPriority(World &world, const GC_Vehicle &veh) const { return AIP_NORMAL; }

	virtual void Detach(World &world);
	virtual void Disappear(World &world);
	virtual float GetDefaultRespawnTime() const = 0;

	// GC_Object
    void Init(World &world) override;
    void Kill(World &world) override;
	void MapExchange(MapFile &f) override;
	void Serialize(World &world, SaveFile &f) override;
	void TimeStep(World &world, float dt) override;
#ifdef NETWORK_DEBUG
	virtual DWORD checksum(void) const
	{
		DWORD cs = reinterpret_cast<const DWORD&>(GetPos().x)
		^ reinterpret_cast<const DWORD&>(GetPos().y)
		^ reinterpret_cast<const DWORD&>(_timeAttached);
		return GC_Actor::checksum() ^ cs;
	}
#endif

protected:
	class MyPropertySet : public GC_Actor::MyPropertySet
	{
		typedef GC_Actor::MyPropertySet BASE;
		ObjectProperty _propTimeRespawn;
		ObjectProperty _propOnPickup;

	public:
		MyPropertySet(GC_Object *object);
		virtual int GetCount() const;
		virtual ObjectProperty* GetProperty(int index);
		virtual void MyExchange(World &world, bool applyToObject);
	};

    PropertySet* NewPropertySet() override;

private:
	ObjPtr<GC_HideLabel>  _label;

	std::string  _scriptOnPickup;   // on_pickup(who)
	float  _timeAttached;
	float  _timeRespawn;

	virtual void OnAttached(World &world, GC_Vehicle &vehicle) = 0;
};

///////////////////////////////////////////////////////////////////////////////

class GC_pu_Health : public GC_Pickup
{
	DECLARE_SELF_REGISTRATION(GC_pu_Health);

public:
	GC_pu_Health(Vector2 pos);
	GC_pu_Health(FromFile);

	// GC_Pickup
	float GetDefaultRespawnTime() const override { return 15.0f; }
	AIPRIORITY GetPriority(World &world, const GC_Vehicle &veh) const override;

protected:
	void OnAttached(World &world, GC_Vehicle &vehicle) override;
};

///////////////////////////////////////////////////////////////////////////////

class GC_pu_Mine : public GC_Pickup
{
	DECLARE_SELF_REGISTRATION(GC_pu_Mine);

public:
	GC_pu_Mine(Vector2 pos);
	GC_pu_Mine(FromFile);

	float GetDefaultRespawnTime() const override { return 15.0f; }
	AIPRIORITY GetPriority(World &world, const GC_Vehicle &veh) const override;

protected:
	void OnAttached(World &world, GC_Vehicle &vehicle) override;
};

///////////////////////////////////////////////////////////////////////////////

#define PROTECT_TIME        20.0f

struct DamageDesc;

class GC_pu_Shield : public GC_Pickup
{
	DECLARE_SELF_REGISTRATION(GC_pu_Shield);

public:
	GC_pu_Shield(Vector2 pos);
	GC_pu_Shield(FromFile);
	virtual ~GC_pu_Shield();

	// GC_Pickup
    void Detach(World &world) override;
    float GetDefaultRespawnTime() const override { return 30.0f; }
    AIPRIORITY GetPriority(World &world, const GC_Vehicle &veh) const override;

	// GC_Object
    void Serialize(World &world, SaveFile &f) override;
    void TimeStep(World &world, float dt) override;

protected:
    void OnAttached(World &world, GC_Vehicle &vehicle) override;

private:
	float _timeHit;
	ObjPtr<GC_Vehicle> _vehicle;

	friend class GC_Vehicle;
	void OnOwnerDamage(World &world, DamageDesc &dd);
};

///////////////////////////////////////////////////////////////////////////////

#define SHOCK_TIMEOUT        1.5f

class GC_pu_Shock : public GC_Pickup
{
	DECLARE_SELF_REGISTRATION(GC_pu_Shock);
    DECLARE_LIST_MEMBER(override);
    typedef GC_Pickup base;

public:
	GC_pu_Shock(Vector2 pos);
	GC_pu_Shock(FromFile);
	virtual ~GC_pu_Shock();

	Vector2 GetTargetPos() const { return _targetPos; }

	// GC_Pickup
    void Detach(World &world) override;
    float GetDefaultRespawnTime() const override { return 15.0f; }
    AIPRIORITY GetPriority(World &world, const GC_Vehicle &veh) const override;

	// GC_Object
    void Kill(World &world) override;
    void Serialize(World &world, SaveFile &f) override;
    void TimeStep(World &world, float dt) override;

protected:
    void OnAttached(World &world, GC_Vehicle &vehicle) override;

private:
	ObjPtr<GC_Light> _light;
	ObjPtr<GC_Vehicle> _vehicle;
	Vector2 _targetPos;

	GC_Vehicle *FindNearVehicle(World &world, const GC_RigidBodyStatic *ignore) const;
};

///////////////////////////////////////////////////////////////////////////////

#define BOOSTER_TIME        20.0f

class GC_pu_Booster : public GC_Pickup
{
	DECLARE_SELF_REGISTRATION(GC_pu_Booster);

public:
	GC_pu_Booster(Vector2 pos);
	GC_pu_Booster(FromFile);
	virtual ~GC_pu_Booster();

	// GC_Pickup
    void Detach(World &world) override;
    float GetDefaultRespawnTime() const override { return 30.0f; }
    AIPRIORITY GetPriority(World &world, const GC_Vehicle &veh) const override;

	// GC_Object
    void Serialize(World &world, SaveFile &f) override;
    void TimeStep(World &world, float dt) override;

protected:
    void OnAttached(World &world, GC_Vehicle &vehicle) override;

private:
	ObjPtr<GC_Weapon> _weapon;

	friend class GC_Weapon;
	void OnWeaponDisappear(World &world, GC_Object *sender, void *param);
};

