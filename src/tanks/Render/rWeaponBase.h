#pragma once
#include "ObjectView.h"
#include "math/Vector2.h"
#include <stddef.h>

class DrawingContext;
class GC_Weapon;

Vector2 GetWeapSpriteDirection(const World &world, const GC_Weapon &weapon);
void DrawWeaponShadow(const World &world, const GC_Weapon &weapon, DrawingContext &dc, size_t texId);

class Z_Weapon : public ObjectZFunc
{
public:
	enumZOrder GetZ(const World &world, const GC_Actor &actor) const override;
};
