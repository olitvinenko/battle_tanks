#pragma once
#include "CampaignControlCommands.h"
#include "InputManager.h"
#include "ctx/GameEvents.h"
#include "gv/GameViewHarness.h"
#include "ui/Window.h"
#include "ui/Text.h"
#include <unordered_map>

class ShellConfig;
class LangCache;
class GameContext;
class World;
class WorldView;
class WorldController;
class DefaultCamera;

class MessageArea;
class ScoreTable;
class CampaignControls;


namespace UI
{
	class ConsoleBuffer;
	class StackLayout;
}

class GameLayout
	: public UI::Window
	, private UI::PointerSink
	, private GameListener
{
public:
	GameLayout(UI::LayoutManager &manager,
	           TextureManager &texman,
	           GameContext &gameContext,
	           WorldView &worldView,
	           WorldController &worldController,
	           ShellConfig &conf,
	           LangCache &lang,
	           UI::ConsoleBuffer &logger,
	           CampaignControlCommands campaignControlCommands);
	virtual ~GameLayout();

	// Window
	void OnTimeStep(UI::LayoutManager &manager, float dt) override;
	void Draw(const UI::StateContext &sc, const UI::LayoutContext &lc, const UI::InputContext &ic, DrawingContext &dc, TextureManager &texman) const override;
	RectFloat GetChildRect(TextureManager &texman, const UI::LayoutContext &lc, const UI::StateContext &sc, const UI::Window &child) const override;
	PointerSink* GetPointerSink() override { return this; }

private:
	void OnChangeShowTime();
	Vector2 GetDragDirection() const;
	unsigned int GetEffectiveDragCount() const;

	std::shared_ptr<MessageArea> _msg;
	std::shared_ptr<UI::StackLayout> _scoreAndControls;
	std::shared_ptr<ScoreTable> _score;
	std::shared_ptr<CampaignControls> _campaignControls;
	std::shared_ptr<UI::Text> _timerDisplay;

	GameContext &_gameContext;
	GameViewHarness _gameViewHarness;
	WorldView &_worldView;
	WorldController &_worldController;
	ShellConfig &_conf;
	LangCache &_lang;
	InputManager _inputMgr;
	size_t _texDrag;
	size_t _texTarget;

	std::unordered_map<unsigned int, std::pair<Vector2, Vector2>> _activeDrags;

	// UI::PointerSink
	bool OnPointerDown(UI::InputContext &ic, UI::LayoutContext &lc, TextureManager &texman, Vector2 pointerPosition, int button, UI::PointerType pointerType, unsigned int pointerID) override;
	void OnPointerUp(UI::InputContext &ic, UI::LayoutContext &lc, TextureManager &texman, Vector2 pointerPosition, int button, UI::PointerType pointerType, unsigned int pointerID) override;
	void OnPointerMove(UI::InputContext &ic, UI::LayoutContext &lc, TextureManager &texman, Vector2 pointerPosition, UI::PointerType pointerType, unsigned int pointerID, bool captured) override;
	void OnTap(UI::InputContext &ic, UI::LayoutContext &lc, TextureManager &texman, Vector2 pointerPosition) override;

	// GameListener
	void OnMurder(GC_Player &victim, GC_Player *killer, MurderType murderType) override;
};
