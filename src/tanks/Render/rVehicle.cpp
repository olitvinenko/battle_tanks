#include "rVehicle.h"
#include <Vehicle.h>
#include <Player.h>
#include <DrawingContext.h>
#include <TextureManager.h>

R_Vehicle::R_Vehicle(TextureManager &tm)
	: _tm(tm)
	, _nameFont(tm.FindSprite("font_small"))
{
}

void R_Vehicle::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	assert(dynamic_cast<const GC_Vehicle*>(&actor));
	auto &vehicle = static_cast<const GC_Vehicle&>(actor);

	vec2d pos = vehicle.GetPos();
	vec2d dir = vehicle.GetDirection();
	float radius = vehicle.GetRadius();
	size_t texId = _tm.FindSprite(vehicle.GetSkin());
	dc.DrawSprite(texId, 0, 0x40000000, pos.x + 4, pos.y + 4, dir);
	dc.DrawSprite(texId, 0, 0xffffffff, pos.x, pos.y, dir);

	if( vehicle.GetOwner() )
	{
		dc.DrawBitmapText(vec2d{ pos.x, pos.y + radius + 4 }, // leave space for ammo indicator
			1.f, _nameFont, 0x7f7f7f7f, vehicle.GetOwner()->GetNick(), alignTextCT);
	}

#ifndef NDEBUG
	//	for( int i = 0; i < 4; ++i )
	//	{
	//		DbgLine(GetVertex(i), GetVertex((i+1)&3));
	//	}
#endif // NDEBUG
}
