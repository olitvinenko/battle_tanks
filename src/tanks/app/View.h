#pragma once
#include <memory>

class TzodApp;
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

class TzodView
{
public:
	TzodView(FileSystem::IFileSystem &fs,
		UI::ConsoleBuffer &logger,
		TzodApp &app,
		AppWindow &appWindow);
	~TzodView();

	void Step(float dt);
	void Render(float pxWidth, float pxHeight, float scale);

private:
	AppWindow &_appWindow;
	std::unique_ptr<struct TzodViewImpl> _impl;
};
