#pragma once

#include "TextureManager.h"
#include "RenderScheme.h"
#include "Base/IWindow.h"


struct IRender;
class DrawingContext;

class RenderingEngine
{
public:
	explicit RenderingEngine(IRender* render, int layersCount, std::shared_ptr<IWindow> window);

	TextureManager& GetTextureManager() { return m_textures; }
	const TextureManager& GetTextureManager() const { return m_textures; }

	RenderScheme& GetScheme()
	{
		return m_scheme;
	}

	void SetMode(RenderMode mode);

	void PreRender();
	void Render(float interpolation);
	void PostRender();

	unsigned int GetPixelWidth() const { return m_window->GetPixelWidth(); }
	unsigned int GetPixelHeight() const { return m_window->GetPixelHeight(); }

	std::shared_ptr<IWindow> GetWindow() const { return m_window; }

private:

	std::shared_ptr<IWindow> m_window;
	IRender* m_render;
	TextureManager m_textures;
	RenderScheme m_scheme;
};
