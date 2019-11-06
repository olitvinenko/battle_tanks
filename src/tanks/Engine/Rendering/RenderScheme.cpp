#include "RenderScheme.h"
#include "IDrawable.h"

#include <cassert>

RenderScheme::RenderScheme(int firstLayer, int lastLayer)
	: m_firstLayer(firstLayer)
	, m_lastLayer(lastLayer)
{
	assert(firstLayer < lastLayer);

	m_drawables = new std::set<const IDrawable*>[lastLayer - firstLayer];
}

RenderScheme::~RenderScheme()
{
	delete[] m_drawables;
}

void RenderScheme::RegisterDrawable(const IDrawable* drawable)
{
	m_drawables[drawable->GetOrder() - m_firstLayer].insert(drawable);
}

void RenderScheme::Draw(DrawingContext& dc, float interpolation) const
{
	for (int i = m_firstLayer; i < m_lastLayer; ++i)
	{
		int index = i - m_firstLayer;

		std::set<const IDrawable*> orderQueue = m_drawables[i];
		for (const IDrawable* d : orderQueue)
			d->Draw(dc, interpolation);
	}
}

void RenderScheme::UnegisterDrawable(const IDrawable* drawable)
{
	assert(m_drawables[drawable->GetOrder() - m_firstLayer].erase(drawable) == 1);
}
