#include "rBooster.h"
#include "gc/Actor.h"
#include "gc/World.h"
#include "rendering/Color.h"
#include "rendering/DrawingContext.h"
#include "rendering/TextureManager.h"

R_Booster::R_Booster(TextureManager &tm)
	: _texId(tm.FindSprite("booster"))
{
}

void R_Booster::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	Vector2 pos = actor.GetPos();
	Vector2 dir = Vec2dDirection(world.GetTime() * 50);
	dc.DrawSprite(_texId, 0, 0xffffffff, pos.x, pos.y, dir);
}
