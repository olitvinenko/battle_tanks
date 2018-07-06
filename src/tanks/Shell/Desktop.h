#pragma once
#include "ConfigConsoleHistory.h"
#include "DefaultCamera.h"
#include "WorldView.h"
#include "UIWindow.h"
#include <memory>
#include <vector>
#include "Widgets.h"
#include "EditorLayout.h"
#include "AppStateListener.h"

namespace FileSystem
{
	class IFileSystem;
}

struct lua_State;

struct LuaStateDeleter
{
	void operator()(lua_State *L) const;
};

namespace UI
{
	class ConsoleBuffer;
	//class Console;
	//class Oscilloscope;
	class ButtonBase;
}

//class AppController;
class MainMenuDlg;
class EditorLayout;
//class GameLayout;
//class FpsCounter;
//class ConfCache;
//class LangCache;

class Desktop
	: public UI::UIWindow
	, private AppStateListener
{
public:
	Desktop(UI::LayoutManager* manager,
	        AppState &appState,
			FileSystem::IFileSystem &fs,
	       // ConfCache &conf,
	       // LangCache &lang,
	        UI::ConsoleBuffer &logger);
	virtual ~Desktop();

	void ShowConsole(bool show);

protected:
	bool OnKeyPressed(UI::Key key) override;
	bool OnFocus(bool focus) override;
	void OnSize(float width, float height) override;
	void OnTimeStep(float dt) override;

private:
	ConfigConsoleHistory  _history;
	FileSystem::IFileSystem &_fs;
	UI::ConsoleBuffer &_logger;
	std::unique_ptr<lua_State, LuaStateDeleter> _globL;

	EditorLayout *_editor = nullptr;
	//GameLayout   *_game = nullptr;
	UI::Console  *_con = nullptr;
	FpsCounter   *_fps = nullptr;
	UI::ButtonBase *_pauseButton = nullptr;
	std::vector<UI::UIWindow*> _navStack;
	float _navTransitionTime = 0;
	float _navTransitionStart = 0;

	//RenderScheme _renderScheme;
	WorldView _worldView;
	DefaultCamera _defaultCamera;

	void OnNewCampaign();
	void OnNewDM();
	void OnNewMap();
	void OnOpenMap(std::string fileName);
	void OnExportMap(std::string fileName);
	void OnGameSettings();

	//network
	void OnHost();
	void OnJoin();

	bool GetEditorMode() const;
	void SetEditorMode(bool editorMode);
	bool IsGamePaused() const;
	void ShowMainMenu();

	void OnChangeShowFps();

	void OnCommand(const std::string &cmd);
	bool OnCompleteCommand(const std::string &cmd, int &pos, std::string &result);

	void OnCloseChild(UI::UIWindow *child, int result);
	void ClearNavStack();
	void PopNavStack(UI::UIWindow *wnd = nullptr);
	void PushNavStack(UI::UIWindow &wnd);

	template <class T>
	bool IsOnTop() const
	{
		return !_navStack.empty() && !!dynamic_cast<T*>(_navStack.back());
	}

	float GetNavStackSize() const;
	float GetTransitionTarget() const;

	// AppStateListener
	void OnGameContextChanging() override;
	void OnGameContextChanged() override;
};
