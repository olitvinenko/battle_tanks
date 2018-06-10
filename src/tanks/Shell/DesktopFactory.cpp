#include "Desktop.h"
#include "DesktopFactory.h"

DesktopFactory::DesktopFactory(//AppState &appState,
                               //AppController &appController,
                               FileSystem::IFileSystem &fs,
                              // ConfCache &conf,
                               //LangCache &lang,
                               UI::ConsoleBuffer &logger)
	:// _appState(appState)
	//, _appController(appController)
	/*,*/ _fs(fs)
	//, _conf(conf)
	//, _lang(lang)
	, _logger(logger)
{
}

UI::UIWindow* DesktopFactory::Create(UI::LayoutManager *manager)
{
	return new Desktop(manager/*, _appState, _appController*/, _fs,/* _conf, _lang,*/ _logger);
}
