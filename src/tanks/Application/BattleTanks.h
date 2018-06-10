#pragma once
#include <memory>

//#include "AppController.h"
//#include "AppState.h"
//#include "../loc/Language.h"
//#include "../shell/Config.h"

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ConsoleBuffer;
}

struct EventPump;
struct AppWindow;

//class ConfCache;

class Application
{
public:
	Application(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger);
	~Application();

	//AppState& GetAppState() const;
	//AppController& GetAppController() const;
	//ConfCache& GetConf() const;
	//LangCache& GetLang() const;

	void Step(float dt) const;
	void Exit() const;

private:
	struct TzodAppImpl
	{
		explicit TzodAppImpl(FileSystem::IFileSystem &fs);

		//ConfCache m_conf;
		//LangCache m_lang;
		//AppState m_appState;
		//AppController m_appController;
	};

	UI::ConsoleBuffer &m_logger;
	std::unique_ptr<TzodAppImpl> m_impl;
};
