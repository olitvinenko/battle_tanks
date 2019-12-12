#pragma once

#include "Terrain.h"
#include "math/Vector2.h"

class DrawingContext;
class TextureManager;
class RenderScheme;
class World;

class WorldView
{
public:
    WorldView(TextureManager &tm, RenderScheme &rs);
	~WorldView();
	void Render(DrawingContext &dc,
				const World &world,
				const RectInt &viewport,
				Vector2 eye,
				float zoom,
				bool editorMode,
				bool drawGrid,
				bool nightMode) const;
	RenderScheme &GetRenderScheme() const { return _renderScheme; }

private:
	RenderScheme &_renderScheme;
	Terrain _terrain;
};
