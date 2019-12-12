#include "rSprite.h"
#include "gc/Actor.h"

#include "rendering/Color.h"
#include "rendering/DrawingContext.h"
#include "rendering/TextureManager.h"

R_Sprite::R_Sprite(TextureManager &tm, const char *tex)
	: _texId(tm.FindSprite(tex))
{
}

void R_Sprite::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	Vector2 pos = actor.GetPos();
	Vector2 dir = actor.GetDirection();
	dc.DrawSprite(_texId, 0, 0xffffffff, pos.x, pos.y, dir);
}
