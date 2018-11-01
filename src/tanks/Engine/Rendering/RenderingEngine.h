#pragma once

#include "TextureManager.h"

struct IRender;
class DrawingContext;

class RenderingEngine
{
public:
	explicit RenderingEngine(IRender* render);

	TextureManager& GetTextureManager() { return m_textures; }
	const TextureManager& GetTextureManager() const { return m_textures; }

	DrawingContext GetContext(unsigned int width, unsigned int height) const;

private:

	IRender* m_render;
	TextureManager m_textures;
};