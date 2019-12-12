#pragma once

#include <set>

class DrawingContext;
struct IDrawable;

class RenderScheme
{
public:
	RenderScheme(int firstLayer, int lastLayer);

	~RenderScheme();

	void RegisterDrawable(const IDrawable* drawable);
	void Draw(DrawingContext& dc, float interpolation) const;
	void UnegisterDrawable(const IDrawable* drawable);

	RenderScheme(const RenderScheme& rs) = delete;
	RenderScheme(RenderScheme&& rs) = delete;

	RenderScheme& operator=(const RenderScheme& rs) = delete;
	RenderScheme& operator=(RenderScheme&& rs) = delete;

private:
    int m_firstLayer;
	int m_lastLayer;

	std::set<const IDrawable*>* m_drawables;
};
