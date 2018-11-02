#pragma once

#include <set>

class DrawingContext;
struct IDrawable;

class RendrerScheme
{
public:
	RendrerScheme(int firstLayer, int lastLayer);

	~RendrerScheme();

	void RegisterDrawable(const IDrawable& drawable);
	void Draw(DrawingContext& dc, float interpolation) const;
	void UnegisterDrawable(const IDrawable& drawable);

	RendrerScheme(const RendrerScheme& rs) = delete;
	RendrerScheme(RendrerScheme&& rs) = delete;

	RendrerScheme& operator=(const RendrerScheme& rs) = delete;
	RendrerScheme& operator=(RendrerScheme&& rs) = delete;

private:
	int m_lastLayer;
	int m_firstLayer;

	std::set<const IDrawable*>* m_drawables;
};
