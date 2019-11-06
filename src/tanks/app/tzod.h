#pragma once
#include <memory>

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

class AppConfig;
class AppState;
class AppController;
class ShellConfig;
class LangCache;
class DMCampaign;

class TzodApp
{
public:
	TzodApp(FileSystem::IFileSystem &fs, UI::ConsoleBuffer &logger, const char *language = nullptr);
	~TzodApp();

	AppState& GetAppState();
	AppController& GetAppController();
	AppConfig& GetAppConfig();
	ShellConfig& GetShellConfig();
	LangCache& GetLang();
	DMCampaign &GetDMCampaign();

	void Step(float dt);
	void Exit();

private:
	UI::ConsoleBuffer &_logger;
	std::unique_ptr<struct TzodAppImpl> _impl;
};
