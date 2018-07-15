#pragma once

#include "Rect.h"

class Pathfinder;
class DrawingContext;
class TextureManager;


class WorldView
{
public:
	void Render(DrawingContext &dc,
				const RectInt &viewport,
				Vector2 eye,
				float zoom,
				bool editorMode,
				bool nightMode,
				Pathfinder& pathfinder) const;
};
