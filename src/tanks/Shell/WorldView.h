#pragma once

#include "Terrain.h"
#include "Rect.h"

class DrawingContext;
class TextureManager;
class World;

class WorldView
{
public:
    WorldView(TextureManager &tm);
	~WorldView();
	void Render(DrawingContext &dc,
				const World &world,
				const RectInt &viewport,
				Vector2 eye,
				float zoom,
				bool editorMode,
				bool drawGrid,
				bool nightMode) const;
private:
	Terrain _terrain;
};
