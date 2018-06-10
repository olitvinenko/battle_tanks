#pragma once
#include <memory>

class Application;
struct AppWindow;
struct IRender;

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class LayoutManager;
	class ConsoleBuffer;
}

class View
{
public:
	View(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, Application &app, AppWindow &appWindow);
	~View();

	void Step(float dt);
	void Render(AppWindow &appWindow);

	UI::LayoutManager& GetGui();

private:
	AppWindow &_appWindow;
	std::unique_ptr<struct TzodViewImpl> _impl;
};
