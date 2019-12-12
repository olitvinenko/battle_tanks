#include "rShock.h"
#include "gc/Pickup.h"

#include "rendering/Color.h"
#include "rendering/DrawingContext.h"
#include "rendering/TextureManager.h"

R_Shock::R_Shock(TextureManager &tm)
	: _texId(tm.FindSprite("lightning"))
{
}

void R_Shock::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	assert(dynamic_cast<const GC_pu_Shock*>(&actor));
	auto &shock = static_cast<const GC_pu_Shock&>(actor);
	if( !shock.GetGridSet() )
	{
		Color c;
		c.r = c.g = c.b = c.a = int((1.0f - ((shock.GetTimeAttached() - SHOCK_TIMEOUT) * 5.0f)) * 255.0f);
		Vector2 pos0 = shock.GetPos();
		Vector2 pos1 = shock.GetTargetPos();
		dc.DrawLine(_texId, c, pos0.x, pos0.y, pos1.x, pos1.y, frand(1));
	}
}
