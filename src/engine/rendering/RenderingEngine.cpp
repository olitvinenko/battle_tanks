#include "RenderingEngine.h"
#include "DrawingContext.h"

RenderingEngine::RenderingEngine(IRender* render, int layersCount, std::shared_ptr<IWindow> window)
	: m_render(render)
	, m_textures(*render)
	, m_scheme(0, layersCount)
	, m_window(window)
{
}

void RenderingEngine::SetMode(RenderMode mode)
{
	m_render->SetMode(mode);
}

void RenderingEngine::PreRender()
{
	m_render->Begin();
}

void RenderingEngine::Render(float interpolation)
{
	DrawingContext dc(m_textures, m_render, GetPixelWidth(), GetPixelHeight());
	m_scheme.Draw(dc, interpolation);
}

void RenderingEngine::PostRender()
{
	m_render->End();
}
