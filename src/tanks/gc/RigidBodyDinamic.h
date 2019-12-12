#pragma once
#include "RigidBody.h"
#include "memory/ObjectPtr.h"
#include <stack>

#define GC_FLAG_RBDYMAMIC_ACTIVE    (GC_FLAG_RBSTATIC_ << 0)
#define GC_FLAG_RBDYMAMIC_PARITY    (GC_FLAG_RBSTATIC_ << 1)
#define GC_FLAG_RBDYMAMIC_          (GC_FLAG_RBSTATIC_ << 2)


class GC_RigidBodyDynamic : public GC_RigidBodyStatic
{
    DECLARE_LIST_MEMBER(override);
    typedef GC_RigidBodyStatic base;

	struct Contact
	{
		ObjPtr<GC_RigidBodyDynamic> obj1_d;
		ObjPtr<GC_RigidBodyStatic>  obj2_s;
		GC_RigidBodyDynamic *obj2_d;
		Vector2 origin;
		Vector2 normal;
		Vector2 tangent;
		float total_np, total_tp;
		float depth;
//		bool  inactive;
	};

	typedef std::vector<Contact> ContactList;
	static ContactList _contacts;
	static std::stack<ContactList> _contactsStack;
	static bool _glob_parity;

	float geta_s(const Vector2 &n, const Vector2 &c, const GC_RigidBodyStatic *obj) const;
	float geta_d(const Vector2 &n, const Vector2 &c, const GC_RigidBodyDynamic *obj) const;

	void impulse(const Vector2 &origin, const Vector2 &impulse);

	bool parity() { return CheckFlags(GC_FLAG_RBDYMAMIC_PARITY); }


	Vector2 _external_force;
	float _external_momentum;
	Vector2 _external_impulse;
	float _external_torque;


	class MyPropertySet : public GC_RigidBodyStatic::MyPropertySet
	{
		typedef GC_RigidBodyStatic::MyPropertySet BASE;
		ObjectProperty _propM;  // mass
		ObjectProperty _propI;  // scalar moment of inertia
		ObjectProperty _propPercussion;
		ObjectProperty _propFragility;
		ObjectProperty _propNx;
		ObjectProperty _propNy;
		ObjectProperty _propNw;
		ObjectProperty _propRotation;
	public:
		MyPropertySet(GC_Object *object);
		virtual int GetCount() const;
		virtual ObjectProperty* GetProperty(int index);
		virtual void MyExchange(World &world, bool applyToObject);
	};

public:
	float GetSpinup() const;
	Vector2 GetBrakingLength() const;

public:
	explicit GC_RigidBodyDynamic(Vector2 pos);
	explicit GC_RigidBodyDynamic(FromFile);

	PropertySet* NewPropertySet() override;
	void MapExchange(MapFile &f) override;
	void Serialize(World &world, SaveFile &f) override;
	void TimeStep(World &world, float dt) override;

	static void ProcessResponse(World &world);
	static void PushState();
	static void PopState();

	float Energy() const;

	float _av;      // angular velocity
	Vector2 _lv;      // linear velocity

	float _inv_m;   // 1/mass
	float _inv_i;   // 1/moment_of_inertia

//	float _Nb;      // braking friction factor X

	float _Nx;      // dry friction factor X
	float _Ny;      // dry friction factor Y
	float _Nw;      // angilar dry friction factor

	float _Mx;      // viscous friction factor X
	float _My;      // viscous friction factor Y
	float _Mw;      // angular viscous friction factor

	float _percussion;  // percussion factor
	float _fragility;   // fragility factor (0 - invulnerability)

	//--------------------------------

	void ApplyMomentum(float momentum);
	void ApplyForce(const Vector2 &force);
	void ApplyForce(const Vector2 &force, const Vector2 &origin);

	void ApplyTorque(float torque);
	void ApplyImpulse(const Vector2 &impulse);
	void ApplyImpulse(const Vector2 &impulse, const Vector2 &origin);

	//--------------------------------

#ifdef NETWORK_DEBUG
public:
	virtual DWORD checksum(void) const
	{
		DWORD cs = reinterpret_cast<const DWORD&>(_av);
		cs ^= reinterpret_cast<const DWORD&>(_lv.x) ^ reinterpret_cast<const DWORD&>(_lv.y);
		cs ^= reinterpret_cast<const DWORD&>(_inv_m) ^ reinterpret_cast<const DWORD&>(_inv_i);

		cs ^= reinterpret_cast<const DWORD&>(_Nx);
		cs ^= reinterpret_cast<const DWORD&>(_Ny);
		cs ^= reinterpret_cast<const DWORD&>(_Nw);

		cs ^= reinterpret_cast<const DWORD&>(_Mx);
		cs ^= reinterpret_cast<const DWORD&>(_My);
		cs ^= reinterpret_cast<const DWORD&>(_Mw);

		cs ^= reinterpret_cast<const DWORD&>(_percussion) ^ reinterpret_cast<const DWORD&>(_fragility);

		cs ^= reinterpret_cast<const DWORD&>(_external_force.x);
		cs ^= reinterpret_cast<const DWORD&>(_external_force.y);
		cs ^= reinterpret_cast<const DWORD&>(_external_momentum);

		cs ^= reinterpret_cast<const DWORD&>(_external_impulse.x);
		cs ^= reinterpret_cast<const DWORD&>(_external_impulse.y);
		cs ^= reinterpret_cast<const DWORD&>(_external_torque);

		return GC_RigidBodyStatic::checksum() ^ cs;
	}
#endif
};

///////////////////////////////////////////////////////////////////////////////
// end of file
