#include "RenderingEngine.h"
#include "DrawingContext.h"

RenderingEngine::RenderingEngine(IRender* render, int layersCount)
	: m_render(render)
	, m_textures(render)
	, m_scheme(0, layersCount)
{
}

void RenderingEngine::PreRender()
{
	m_render->Begin();
}

void RenderingEngine::Render(unsigned int width, unsigned int height, float interpolation)
{
	DrawingContext dc(m_textures, m_render, width, height);
	m_scheme.Draw(dc, interpolation);
}

void RenderingEngine::PostRender()
{
	m_render->End();
}
