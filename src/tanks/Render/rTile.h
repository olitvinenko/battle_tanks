#pragma once
#include "ObjectView.h"
#include "rendering/Color.h"
#include "math/Vector2.h"

#include <stddef.h>

class TextureManager;

class R_Tile : public ObjectRFunc
{
public:
	R_Tile(TextureManager &tm, const char *tex, Color color, Vector2 offset);
	void Draw(const World &world, const GC_Actor &actor, DrawingContext &dc) const override;

private:
	size_t _texId;
	Color _color;
	Vector2 _offset;
};
