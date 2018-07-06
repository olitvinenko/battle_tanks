#pragma once
#include "GuiManager.h"

class AppState;

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ConsoleBuffer;
}

class DesktopFactory : public UI::IWindowFactory
{
public:
	DesktopFactory(AppState &appState, FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger);
	UI::UIWindow* Create(UI::LayoutManager *manager) override;

private:
	AppState &_appState;
	FileSystem::IFileSystem &_fs;
	UI::ConsoleBuffer &_logger;
};
