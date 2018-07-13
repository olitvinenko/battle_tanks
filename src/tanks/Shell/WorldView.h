#pragma once

#include "Rect.h"

class DrawingContext;
class TextureManager;
//class Raven_Game;


class WorldView
{
public:
	void Render(DrawingContext &dc,
				const RectInt &viewport,
				Vector2 eye,
				float zoom,
				bool editorMode,
				bool nightMode/*,
				Raven_Game& game*/) const;
};
