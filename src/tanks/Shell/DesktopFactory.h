#pragma once
#include "GuiManager.h"


//class AppState;
//class AppController;
//class ConfCache;
//class LangCache;

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
	DesktopFactory(//AppState &appState,
	               //AppController &appController,
				   FileSystem::IFileSystem &fs,
	               //ConfCache &conf,
	               //LangCache &lang,
	               UI::ConsoleBuffer &logger);

	UI::UIWindow* Create(UI::LayoutManager *manager) override;

private:
	//AppState &_appState;
	//AppController &_appController;
	FileSystem::IFileSystem &_fs;
	//ConfCache &_conf;
	//LangCache &_lang;
	UI::ConsoleBuffer &_logger;
};
