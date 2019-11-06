#pragma once
#include <string>

class AppState;
class AppConfig;
class DMCampaign;
struct DMSettings;

namespace FileSystem
{
	class IFileSystem;
}

class AppController
{
public:
	AppController(FileSystem::IFileSystem &fs);
	void Step(AppState &appState, AppConfig &appConfig, float dt);
    
    void NewGameDM(AppState &appState, const std::string &mapName, const DMSettings &settings);
	void StartDMCampaignMap(AppState &appState, AppConfig &appConfig, DMCampaign &dmCampaign, unsigned int tier, unsigned int map);

private:
	FileSystem::IFileSystem &_fs;
};
