#include "rLight.h"

#include "gc/Light.h"

#include "rendering/Color.h"
#include "rendering/DrawingContext.h"
#include "rendering/TextureManager.h"

enumZOrder Z_Light::GetZ(const World &world, const GC_Actor &actor) const
{
	assert(dynamic_cast<const GC_Light*>(&actor));
	auto &light = static_cast<const GC_Light&>(actor);
	return GC_Light::LIGHT_SPOT == light.GetLightType() && light.GetActive() ? Z_PARTICLE : Z_NONE;
}

R_Light::R_Light(TextureManager &tm)
	: _texId(tm.FindSprite("shine"))
{
}

void R_Light::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	assert(dynamic_cast<const GC_Light*>(&actor));
	auto &light = static_cast<const GC_Light&>(actor);
	Vector2 pos = light.GetPos();
	dc.DrawSprite(_texId, 0, 0xffffffff, pos.x, pos.y, Vector2{ 0, 1 });
}
