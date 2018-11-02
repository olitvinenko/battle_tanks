#pragma once

#include "TextureManager.h"
#include "RendrerScheme.h"


struct IRender;
class DrawingContext;

class RenderingEngine
{
public:
	explicit RenderingEngine(IRender* render, int layersCount);

	TextureManager& GetTextureManager() { return m_textures; }
	const TextureManager& GetTextureManager() const { return m_textures; }

	RendrerScheme& GetScheme()
	{
		return m_scheme;
	}

	void PreRender();
	void Render(unsigned int width, unsigned int height, float interpolation);
	void PostRender();

private:

	IRender* m_render;
	TextureManager m_textures;
	RendrerScheme m_scheme;
};