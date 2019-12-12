#pragma once

#include <stddef.h>
#include "math/Rect.h"

class TextureManager;
class DrawingContext;

class Terrain
{
public:
	Terrain(TextureManager &tm);
	void Draw(DrawingContext &dc, const RectFloat &bounds, bool drawGrid) const;

private:
	size_t _texBack;
	size_t _texGrid;
};
