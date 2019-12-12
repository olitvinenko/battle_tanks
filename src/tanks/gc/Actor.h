#pragma once
#include "Object.h"
#include "math/Vector2.h"
#include "math/Rect.h"

#define GC_FLAG_ACTOR_INGRIDSET     (GC_FLAG_OBJECT_ << 0)
#define GC_FLAG_ACTOR_              (GC_FLAG_OBJECT_ << 1)

class GC_Actor : public GC_Object
{
public:
	explicit GC_Actor(Vector2 pos);
	explicit GC_Actor(FromFile) {}

	const Vector2& GetDirection() const { return _direction; }
	void SetDirection(const Vector2 &d) { assert(fabs(d.sqr()-1)<1e-5); _direction = d; }

	void SetGridSet(bool bGridSet) { SetFlags(GC_FLAG_ACTOR_INGRIDSET, bGridSet); }
	bool GetGridSet() const { return CheckFlags(GC_FLAG_ACTOR_INGRIDSET); }

	const Vector2& GetPos() const { return _pos; }
	virtual void MoveTo(World &world, const Vector2 &pos);

	// GC_Object
	virtual void Init(World &world);
    virtual void Kill(World &world);
	virtual void MapExchange(MapFile &f);
	virtual void Serialize(World &world, SaveFile &f);

protected:
	int _locationX;
	int _locationY;
	virtual void EnterContexts(World &, int locX, int locY);
	virtual void LeaveContexts(World &, int locX, int locY);

private:
	Vector2 _pos;
	Vector2 _direction;
};


#define DECLARE_GRID_MEMBER()                                               \
protected:                                                                  \
    void EnterContexts(World &world, int locX, int locY) override;          \
    void LeaveContexts(World &world, int locX, int locY) override;          \
private:

#define IMPLEMENT_GRID_MEMBER(cls, grid)                                    \
    void cls::EnterContexts(World &world, int locX, int locY)               \
    {                                                                       \
        base::EnterContexts(world, locX, locY);                             \
        world.grid.element(locX, locY).insert(this);                        \
    }                                                                       \
    void cls::LeaveContexts(World &world, int locX, int locY)               \
    {                                                                       \
        auto &cell = world.grid.element(locX, locY);                        \
        for (auto id = cell.begin(); id != cell.end(); id = cell.next(id))  \
        {                                                                   \
            if (cell.at(id) == this)                                        \
            {                                                               \
                cell.erase(id);                                             \
                base::LeaveContexts(world, locX, locY);                     \
                return;                                                     \
            }                                                               \
        }                                                                   \
        assert(false);                                                      \
    }

