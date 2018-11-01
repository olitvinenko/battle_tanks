#pragma once

#include <stack>
#include <string>

#include "Rect.h"
#include "IRender.h"

struct Color;
class TextureManager;


enum AlignTextKind
{
	  alignTextLT = 0
	, alignTextCT = 1
	, alignTextRT = 2
	, alignTextLC = 3
	, alignTextCC = 4
	, alignTextRC = 5
	, alignTextLB = 6
	, alignTextCB = 7
	, alignTextRB = 8
};

class DrawingContext
{
public:
	DrawingContext(const TextureManager &tm, IRender* render, unsigned int width, unsigned int height);

	void PushClippingRect(math::RectInt rect);
	void PopClippingRect();

	void PushTransform(Vector2 offset);
	void PopTransform();

	void DrawSprite(const math::RectFloat *dst, size_t sprite, Color color, unsigned int frame);
	void DrawBorder(const math::RectFloat &dst, size_t sprite, Color color, unsigned int frame);
	void DrawBitmapText(float sx, float sy, size_t tex, Color color, const std::string &str, AlignTextKind align = alignTextLT);
	void DrawSprite(size_t tex, unsigned int frame, Color color, float x, float y, Vector2 dir);
	void DrawSprite(size_t tex, unsigned int frame, Color color, float x, float y, float width, float height, Vector2 dir);
	void DrawIndicator(size_t tex, float x, float y, float value) const;
	void DrawLine(size_t tex, Color color, float x0, float y0, float x1, float y1, float phase);
	void DrawBackground(size_t tex, float sizeX, float sizeY) const;

	void DrawPointLight(float intensity, float radius, Vector2 pos);
	void DrawSpotLight(float intensity, float radius, Vector2 pos, Vector2 dir, float offset, float aspect);
	void DrawDirectLight(float intensity, float radius, Vector2 pos, Vector2 dir, float length);

	void Camera(const math::RectInt &viewport, float x, float y, float scale);
	void SetAmbient(float ambient);
	void SetMode(RenderMode mode);

private:
	const TextureManager &_tm;

	IRender* m_render;
	std::stack<math::RectInt> _clipStack;
	std::stack<Vector2> _transformStack;
	math::RectInt _viewport;
};
