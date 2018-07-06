#include "Desktop.h"
#include "DesktopFactory.h"

DesktopFactory::DesktopFactory(AppState &appState, FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger)
	: _appState(appState)
	, _fs(fs)
	, _logger(logger)
{
}

UI::UIWindow* DesktopFactory::Create(UI::LayoutManager *manager)
{
	return new Desktop(manager, _appState, _fs, _logger);
}
