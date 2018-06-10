#pragma once

#include "TextureManager.h"
#include "GuiManager.h"

class Application;
struct AppWindow;
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
{
public:
	View(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, Application &app, AppWindow &appWindow);
	~View();

	void Step(float dt);
	void Render(AppWindow &appWindow) const;

	UI::LayoutManager& GetGui();

private:
	AppWindow &_appWindow;

	TextureManager textureManager;
	UI::LayoutManager gui;
};
