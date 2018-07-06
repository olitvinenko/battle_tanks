#pragma once
#include "../ui/UIWindow.h"
#include <functional>
#include "GetFileName.h"

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ConsoleBuffer;
	class Text;
}

struct MainMenuCommands
{
	std::function<void()> newCampaign;
	std::function<void()> newDM;
	std::function<void()> newMap;
	std::function<void(std::string)> openMap;
	std::function<void(std::string)> exportMap;
	std::function<void()> gameSettings;
	std::function < void() > close;

	//network
	std::function < void() > onJoin;
	std::function < void() > onHost;
};

class MainMenuDlg : public UI::UIWindow
{
	void OnEditor();
	void OnMapSettings();
	void OnImportMap();
	void OnImportMapSelect(int result);
	void OnExportMap();
	void OnExportMapSelect(int result);

	void OnSettings();


	enum PanelType
	{
		PT_NONE,
		PT_EDITOR,
	};

	enum PanelState
	{
		PS_NONE,
		PS_APPEARING,
		PS_DISAPPEARING,
	};

	UI::UIWindow    *_panel = nullptr;
	UI::UIWindow    *_panelFrame = nullptr;
	UI::Text      *_panelTitle = nullptr;
	PanelType  _ptype;
	PanelState _pstate;

	GetFileNameDlg *_fileDlg;
	FileSystem::IFileSystem &_fs;
	UI::ConsoleBuffer &_logger;
	MainMenuCommands _commands;

public:
	MainMenuDlg(UIWindow *parent,
				FileSystem::IFileSystem &fs,
	            UI::ConsoleBuffer &logger,
	            MainMenuCommands commands);
	virtual ~MainMenuDlg();
	bool OnKeyPressed(UI::Key key) override;
	bool OnFocus(bool) override { return true; }

protected:
	void OnTimeStep(float dt) override;
	void OnCloseChild(int result);
	void CreatePanel(); // create panel of current _ptype and go to PS_APPEARING state
	void SwitchPanel(PanelType newtype);
};
