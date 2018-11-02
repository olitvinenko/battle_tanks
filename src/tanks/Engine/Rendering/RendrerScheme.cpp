#include "RendrerScheme.h"
#include "IDrawable.h"

#include <cassert>

RendrerScheme::RendrerScheme(int firstLayer, int lastLayer)
{
	assert(firstLayer < lastLayer);

	m_drawables = new std::set<const IDrawable*>[lastLayer - firstLayer];
}

RendrerScheme::~RendrerScheme()
{
	delete[] m_drawables;
}

void RendrerScheme::RegisterDrawable(const IDrawable& drawable)
{
	m_drawables[drawable.GetOrder() - m_firstLayer].insert(&drawable);
}

void RendrerScheme::Draw(DrawingContext& dc, float interpolation) const
{
	for (int i = m_firstLayer; i < m_lastLayer; ++i)
	{
		int index = i - m_firstLayer;

		std::set<const IDrawable*> orderQueue = m_drawables[i];
		for (const IDrawable* d : orderQueue)
			d->Draw(dc, interpolation);
	}
}

void RendrerScheme::UnegisterDrawable(const IDrawable& drawable)
{
	m_drawables[drawable.GetOrder() - m_firstLayer].erase(&drawable);
}