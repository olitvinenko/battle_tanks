#include "rAnimatedSprite.h"
#include "gc/Actor.h"
#include "gc/World.h"

#include "rendering/Color.h"
#include "rendering/TextureManager.h"
#include "rendering/DrawingContext.h"

R_AnimatedSprite::R_AnimatedSprite(TextureManager &tm, const char *tex, float frameRate)
	: _tm(tm)
	, _texId(tm.FindSprite(tex))
	, _frameRate(frameRate)
{
}

void R_AnimatedSprite::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	Vector2 pos = actor.GetPos();
	Vector2 dir = actor.GetDirection();
	unsigned int frame = static_cast<unsigned int>(world.GetTime() * _frameRate) % _tm.GetFrameCount(_texId);
	dc.DrawSprite(_texId, frame, 0xffffffff, pos.x, pos.y, dir);
}
