#include "rBooster.h"
#include <Actor.h>
#include <World.h>
#include <TextureManager.h>
#include <DrawingContext.h>

R_Booster::R_Booster(TextureManager &tm)
	: _texId(tm.FindSprite("booster"))
{
}

void R_Booster::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	vec2d pos = actor.GetPos();
	vec2d dir = Vec2dDirection(world.GetTime() * 50);
	dc.DrawSprite(_texId, 0, 0xffffffff, pos.x, pos.y, dir);
}
