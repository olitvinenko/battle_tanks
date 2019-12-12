#include "rMinigun.h"
#include "rWeaponBase.h"

#include "gc/Weapons.h"
#include "gc/WeapCfg.h"
#include "gc/World.h"

#include "rendering/Color.h"
#include "rendering/DrawingContext.h"
#include "rendering/TextureManager.h"

R_WeaponMinigun::R_WeaponMinigun(TextureManager &tm)
	: _texId1(tm.FindSprite("weap_mg1"))
	, _texId2(tm.FindSprite("weap_mg2"))
{
}

void R_WeaponMinigun::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	assert(dynamic_cast<const GC_Weap_Minigun*>(&actor));
	auto &minigun = static_cast<const GC_Weap_Minigun&>(actor);

	Vector2 pos = minigun.GetPos();
	Vector2 dir = GetWeapSpriteDirection(world, minigun);
	size_t texId = minigun.GetFire() ? ((fmod(world.GetTime(), 0.1f) < 0.05f) ? _texId1 : _texId2) : _texId2;
	DrawWeaponShadow(world, minigun, dc, texId);
	dc.DrawSprite(texId, 0, 0xffffffff, pos.x, pos.y, dir);
}


R_Crosshair2::R_Crosshair2(TextureManager &tm)
	: _texId(tm.FindSprite("indicator_crosshair2"))
{
}

void R_Crosshair2::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	assert(dynamic_cast<const GC_Weap_Minigun*>(&actor));
	auto &minigun = static_cast<const GC_Weap_Minigun&>(actor);
	if (minigun.GetAttached())
	{
		Vector2 delta = Vec2dDirection(minigun.GetHeat(world) * 0.1f / WEAP_MG_TIME_RELAX);
		Vector2 dir1 = Vec2dAddDirection(minigun.GetDirection(), delta);
		Vector2 dir2 = Vec2dSubDirection(minigun.GetDirection(), delta);
		Vector2 pos1 = minigun.GetPos() + dir1 * 150.0f;
		Vector2 pos2 = minigun.GetPos() + dir2 * 150.0f;
		dc.DrawSprite(_texId, 0, 0xffffffff, pos1.x, pos1.y, dir1);
		dc.DrawSprite(_texId, 0, 0xffffffff, pos2.x, pos2.y, dir2);
	}
}
