#include "RenderingEngine.h"
#include "DrawingContext.h"

RenderingEngine::RenderingEngine(IRender* render)
	: m_render(render)
	, m_textures(render)
{

}

DrawingContext RenderingEngine::GetContext(unsigned width, unsigned height) const
{
	return DrawingContext(m_textures, m_render, width, height);
}
