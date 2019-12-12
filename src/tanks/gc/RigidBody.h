#pragma once
#include "Actor.h"

#define GC_FLAG_RBSTATIC_TRACE0     (GC_FLAG_ACTOR_ << 0) // penetration of projectiles
#define GC_FLAG_RBSTATIC_DESTROYED  (GC_FLAG_ACTOR_ << 1)
#define GC_FLAG_RBSTATIC_           (GC_FLAG_ACTOR_ << 2)

class GC_Player;

struct DamageDesc
{
	float  damage;
	Vector2  hit;
	GC_Player *from;
};

class GC_RigidBodyStatic : public GC_Actor
{
	DECLARE_GRID_MEMBER();
	typedef GC_Actor base;

public:
	explicit GC_RigidBodyStatic(Vector2 pos);
	explicit GC_RigidBodyStatic(FromFile);
	virtual ~GC_RigidBodyStatic();

	const std::string& GetOnDestroy() const { return _scriptOnDestroy; }
	const std::string& GetOnDamage() const { return _scriptOnDamage; }

	void SetHealth(float cur, float max);
	void SetHealth(float hp);
	void SetHealthMax(float hp);
	float GetHealth() const { return _health; }
	float GetHealthMax() const { return _health_max; }

	void SetSize(float width, float length);
	float GetHalfWidth() const { return _width/2; }
	float GetHalfLength() const { return _length/2; }
	float GetRadius() const { return _radius; }
	Vector2 GetVertex(int index) const;
	bool GetTrace0() const { return CheckFlags(GC_FLAG_RBSTATIC_TRACE0); }

	virtual bool IntersectWithLine(const Vector2 &lineCenter, const Vector2 &lineDirection, Vector2 &outEnterNormal, float &outEnter, float &outExit) const;
	virtual bool IntersectWithRect(const Vector2 &rectHalfSize, const Vector2 &rectCenter, const Vector2 &rectDirection, Vector2 &outWhere, Vector2 &outNormal, float &outDepth) const;

	// return true if object has been killed
	void TakeDamage(World &world, DamageDesc dd);

	virtual float GetDefaultHealth() const = 0;
	virtual unsigned char GetPassability() const = 0;
	virtual GC_Player* GetOwner() const { return nullptr; }

	// GC_Actor
	void MoveTo(World &world, const Vector2 &pos) override;

	// GC_Object
	void Init(World &world) override;
	void Kill(World &world) override;
	void MapExchange(MapFile &f) override;
	void Serialize(World &world, SaveFile &f) override;
#ifdef NETWORK_DEBUG
	virtual DWORD checksum(void) const
	{
		DWORD cs = reinterpret_cast<const DWORD&>(GetPos().x) ^ reinterpret_cast<const DWORD&>(GetPos().y);
		cs ^= reinterpret_cast<const DWORD&>(_health);
		cs ^= reinterpret_cast<const DWORD&>(_width) ^ reinterpret_cast<const DWORD&>(_length);
		return GC_Actor::checksum() ^ cs;
	}
#endif

protected:
	class MyPropertySet : public GC_Actor::MyPropertySet
	{
		typedef GC_Actor::MyPropertySet BASE;
		ObjectProperty _propOnDestroy;
		ObjectProperty _propOnDamage;
		ObjectProperty _propHealth;
		ObjectProperty _propMaxHealth;
	public:
		MyPropertySet(GC_Object *object);
		virtual int GetCount() const;
		virtual ObjectProperty* GetProperty(int index);
		virtual void MyExchange(World &world, bool applyToObject);
	};
	PropertySet* NewPropertySet() override;
	virtual void OnDestroy(World &world, const DamageDesc &dd);
	virtual void OnDamage(World &world, DamageDesc &damageDesc);

private:
	float _health;
	float _health_max;
	float _radius;     // cached radius of bounding sphere
	float _width;
	float _length;
	std::string _scriptOnDestroy;  // on_destroy()
	std::string _scriptOnDamage;   // on_damage()
};
