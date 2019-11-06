#include "rSprite.h"
#include <Actor.h>
#include <TextureManager.h>
#include <DrawingContext.h>

R_Sprite::R_Sprite(TextureManager &tm, const char *tex)
	: _texId(tm.FindSprite(tex))
{
}

void R_Sprite::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	vec2d pos = actor.GetPos();
	vec2d dir = actor.GetDirection();
	dc.DrawSprite(_texId, 0, 0xffffffff, pos.x, pos.y, dir);
}
