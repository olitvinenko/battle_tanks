#pragma once
#include "ScriptMessageBroadcaster.h"
#include "GameContextBase.h"
#include "GameEvents.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

class World;
struct Gameplay;
class AIManager;
class ScriptHarness;
class ThemeManager;
class TextureManager;
class WorldController;
class LangCache;

struct PlayerDesc
{
	std::string nick;
	std::string skin;
	std::string cls;
	unsigned int team;
};

struct DMSettings
{
	std::vector<PlayerDesc> players;
	std::vector<PlayerDesc> bots;
	int fragLimit = 0;
	float timeLimit = 0;
};
namespace FileSystem
{
    class Stream;
}


class GameContext : public GameContextBase
{
public:
	GameContext(FileSystem::Stream &map, const DMSettings &settings, int campaignTier, int campaignMap);
	virtual ~GameContext();
	WorldController& GetWorldController() { return *_worldController; }
	GameEventSource& GetGameEventSource() { return _gameEventsBroadcaster; }
	ScriptMessageSource& GetScriptMessageSource() { return _scriptMessageBroadcaster; }

	void Serialize(FileSystem::Stream &stream);
	void Deserialize(FileSystem::Stream &stream);

	int GetCampaignTier() const { return _campaignTier; }
	int GetCampaignMap() const { return _campaignMap; }

	// GameContextBase
	World& GetWorld() override { return *_world; }
	Gameplay* GetGameplay() override;
	void Step(float dt) override;

private:
	GameEventsBroadcaster _gameEventsBroadcaster;
	app_detail::ScriptMessageBroadcaster _scriptMessageBroadcaster;
	std::unique_ptr<World> _world;
	std::unique_ptr<WorldController> _worldController;
	std::unique_ptr<Gameplay> _gameplay;
	std::unique_ptr<ScriptHarness> _scriptHarness;
	std::unique_ptr<AIManager> _aiManager;

	int _campaignTier;
	int _campaignMap;
};
