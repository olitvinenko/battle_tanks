#include "rIndicator.h"

#include "gc/Vehicle.h"
#include "gc/Weapons.h"
#include "gc/World.h"

#include "rendering/Color.h"
#include "rendering/DrawingContext.h"
#include "rendering/TextureManager.h"

R_HealthIndicator::R_HealthIndicator(TextureManager &tm, bool dynamic)
	: _tm(tm)
	, _texId(tm.FindSprite("indicator_health"))
	, _dynamic(dynamic)
{
}

void R_HealthIndicator::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	assert(dynamic_cast<const GC_RigidBodyStatic*>(&actor));
	auto &rigidBody = static_cast<const GC_RigidBodyStatic&>(actor);

	Vector2 pos = rigidBody.GetPos();
	float radius = _dynamic ? rigidBody.GetRadius() : rigidBody.GetHalfWidth();
	float val = rigidBody.GetHealth() / rigidBody.GetHealthMax();
	dc.DrawIndicator(_texId, pos.x, pos.y - radius - _tm.GetFrameHeight(_texId, 0), val);
}


static void DrawWeaponIndicator(const World &world,
								const TextureManager &tm,
								DrawingContext &dc,
								size_t texId,
								const GC_Weapon &weapon,
								float value)
{
	if( GC_Vehicle *vehicle = weapon.GetVehicle() )
	{
		Vector2 pos = vehicle->GetPos();
		float radius = vehicle->GetRadius();
		dc.DrawIndicator(texId, pos.x, pos.y + radius, value);
	}
}


R_AmmoIndicator::R_AmmoIndicator(TextureManager &tm)
	: _tm(tm)
	, _texId(tm.FindSprite("indicator_ammo"))
{
}

void R_AmmoIndicator::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	assert(dynamic_cast<const GC_ProjectileBasedWeapon*>(&actor));
	auto &weapon = static_cast<const GC_ProjectileBasedWeapon&>(actor);
	float value = 1 - (float) weapon.GetNumShots() / (float) weapon.GetSeriesLength();
	DrawWeaponIndicator(world, _tm, dc, _texId, weapon, value);
}


R_FuelIndicator::R_FuelIndicator(TextureManager &tm)
	: _tm(tm)
	, _texId(tm.FindSprite("indicator_fuel"))
{
}

void R_FuelIndicator::Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const
{
	assert(dynamic_cast<const GC_Weap_Ram*>(&actor));
	auto &ram = static_cast<const GC_Weap_Ram&>(actor);
	DrawWeaponIndicator(world, _tm, dc, _texId, ram, ram.GetFuel() / ram.GetFuelMax());
}
