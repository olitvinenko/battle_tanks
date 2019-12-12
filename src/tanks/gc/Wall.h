#pragma once
#include "RigidBody.h"

#define GC_FLAG_WALL_CORNER_1 (GC_FLAG_RBSTATIC_ << 0) //  0-----1
#define GC_FLAG_WALL_CORNER_2 (GC_FLAG_RBSTATIC_ << 1) //  |     |
#define GC_FLAG_WALL_CORNER_3 (GC_FLAG_RBSTATIC_ << 2) //  |     |
#define GC_FLAG_WALL_CORNER_4 (GC_FLAG_RBSTATIC_ << 3) //  3-----2
#define GC_FLAG_WALL_STYLE_BIT_0  (GC_FLAG_RBSTATIC_ << 4)
#define GC_FLAG_WALL_STYLE_BIT_1  (GC_FLAG_RBSTATIC_ << 5)
#define GC_FLAG_WALL_             (GC_FLAG_RBSTATIC_ << 6)

#define GC_FLAG_WALL_CORNER_ALL   (GC_FLAG_WALL_CORNER_1\
                                  |GC_FLAG_WALL_CORNER_2\
                                  |GC_FLAG_WALL_CORNER_3\
                                  |GC_FLAG_WALL_CORNER_4)


class GC_Wall : public GC_RigidBodyStatic
{
	DECLARE_GRID_MEMBER();
	DECLARE_SELF_REGISTRATION(GC_Wall);
    typedef GC_RigidBodyStatic base;

public:
	explicit GC_Wall(Vector2 pos);
	GC_Wall(FromFile);
	virtual ~GC_Wall();

	void SetCorner(World &world, unsigned int index); // 01
	unsigned int GetCorner(void) const; // 32

	void SetStyle(int style); // 0-3
	int GetStyle() const;

	// GC_RigidBodyStatic
	bool IntersectWithLine(const Vector2 &lineCenter, const Vector2 &lineDirection, Vector2 &outEnterNormal, float &outEnter, float &outExit) const override;
	bool IntersectWithRect(const Vector2 &rectHalfSize, const Vector2 &rectCenter, const Vector2 &rectDirection, Vector2 &outWhere, Vector2 &outNormal, float &outDepth) const override;
	float GetDefaultHealth() const override { return 50; }
	unsigned char GetPassability() const override { return 1; }

	// GC_Object
	void Init(World &world) override;
	void Kill(World &world) override;
	void MapExchange(MapFile &f) override;
	void Serialize(World &world, SaveFile &f) override;

protected:
	class MyPropertySet : public GC_RigidBodyStatic::MyPropertySet
	{
		typedef GC_RigidBodyStatic::MyPropertySet BASE;
		ObjectProperty _propCorner;
		ObjectProperty _propStyle;
	public:
		MyPropertySet(GC_Object *object);
		virtual int GetCount() const;
		virtual ObjectProperty* GetProperty(int index);
		virtual void MyExchange(World &world, bool applyToObject);
	};
	PropertySet* NewPropertySet() override;

	void OnDestroy(World &world, const DamageDesc &dd) override;
	void OnDamage(World &world, DamageDesc &dd) override;
};

/////////////////////////////////////////////////////////////

class GC_Wall_Concrete : public GC_Wall
{
	DECLARE_SELF_REGISTRATION(GC_Wall_Concrete);

public:
	GC_Wall_Concrete(Vector2 pos);
	GC_Wall_Concrete(FromFile) : GC_Wall(FromFile()) {}

	unsigned char GetPassability() const override { return 0xFF; } // impassable

protected:
	void OnDamage(World &world, DamageDesc &dd) override;
};
