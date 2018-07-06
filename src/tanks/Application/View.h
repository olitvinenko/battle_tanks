#pragma once

#include "TextureManager.h"
#include "GuiManager.h"
#include "GameLoop.h"

class AppController;
struct AppWindowBase;
struct IRender;

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ConsoleBuffer;
}

class View
	: public IFixedUpdatable
	, public IRenderable
{
public:
	View(GameLoop &loop, FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, AppController &app, AppWindowBase &appWindow);
	~View();

	void FixedUpdate(float fixedDeltaTime) override;
	void Render(float interpolation) const override;

	UI::LayoutManager& GetGui();

private:
	GameLoop &m_loop;
	AppWindowBase &m_appWindow;

	TextureManager m_textureManager;
	UI::LayoutManager m_gui;
};
