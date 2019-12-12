#include "rBrickFragment.h"
#include "RenderCfg.h"

#include "gc/Actor.h"
#include "gc/World.h"
#include "rendering/Color.h"
#include "rendering/DrawingContext.h"
#include "rendering/TextureManager.h"


R_BrickFragment::R_BrickFragment(TextureManager &tm)
	: _tm(tm)
	, _texId(tm.FindSprite("particle_brick"))
{
}

void R_BrickFragment::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	auto idAsSeed = actor.GetId();
	uint32_t seed = reinterpret_cast<const uint32_t&>(idAsSeed);
	uint32_t rand = ((uint64_t)seed * 279470273UL) % 4294967291UL;

	Vector2 pos = actor.GetPos();
	Vector2 dir = Vec2dDirection((float) (int32_t) rand);
	unsigned int frame = (rand + static_cast<unsigned int>(world.GetTime() * ANIMATION_FPS)) % _tm.GetFrameCount(_texId);
	dc.DrawSprite(_texId, frame, 0xffffffff, pos.x, pos.y, dir);
}
