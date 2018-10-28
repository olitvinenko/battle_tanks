#include "Desktop.h"
//#include "Editor.h"
//#include "GetFileName.h"
//#include "gui.h"
#include "MainMenu.h"
//#include "Network.h"

//#include "../as/AppConfig.h"
#include "FileSystem.h"
//nclude "../loc/Language.h"
#include "ConsoleBuffer.h"
#include "GuiManager.h"
#include "Button.h"
#include "Text.h"
#include "Keys.h"
#include "Config.h"
#include "Dialog.h"

MainMenuDlg::MainMenuDlg(UIWindow *parent,
                         FileSystem::IFileSystem &fs,
                         //ConfCache &conf,
                         //LangCache &lang,
                         UI::ConsoleBuffer &logger,
                         MainMenuCommands commands)
  : UIWindow(parent)
  , _panel(nullptr)
  , _ptype(PT_NONE)
  , _pstate(PS_NONE)
  , _fileDlg(nullptr)
  , _fs(fs)
  //, _lang(lang)
  , _logger(logger)
  , _commands(std::move(commands))
{
	Resize(640, 300);

	UI::Button *button;

	const float buttonWidth = 200;
	const float buttonHeight = 128;

	float y = GetHeight() - buttonHeight;

	button = UI::Button::Create(this, "Single player", 0, y);
	button->SetIcon("ui/play");
	button->Resize(buttonWidth, buttonHeight);
	button->eventClick = _commands.newDM;

	button = UI::Button::Create(this, "Editor", (GetWidth() - buttonWidth) / 2, y);
	button->SetIcon("ui/editor");
	button->Resize(buttonWidth, buttonHeight);
	button->eventClick = std::bind(&MainMenuDlg::OnEditor, this);

	button = UI::Button::Create(this, "Settings", GetWidth() - buttonWidth, y);
	button->SetIcon("ui/settings");
	button->Resize(buttonWidth, buttonHeight);
	button->eventClick = std::bind(&MainMenuDlg::OnSettings, this);

	button = UI::Button::Create(this, "Return", (GetWidth() - buttonWidth) / 2, 0);
	button->Resize(buttonWidth, buttonHeight);
	button->eventClick = _commands.close;

	button = UI::Button::Create(this, "Host", (GetWidth() - buttonWidth), y * 2);
	button->Resize(buttonWidth, buttonHeight);
	button->eventClick = _commands.onHost;

	button = UI::Button::Create(this, "Join", (GetWidth() - buttonWidth), y * 2 + buttonHeight);
	button->Resize(buttonWidth, buttonHeight);
	button->eventClick = _commands.onJoin;

	_panelFrame = UIWindow::Create(this);
	_panelFrame->SetDrawBackground(false);
	_panelFrame->SetDrawBorder(false);
	_panelFrame->SetClipChildren(true);
	_panelFrame->Move(0, GetHeight() + 40);
	_panelFrame->Resize(GetWidth(), 64);

	_panel = UIWindow::Create(_panelFrame);
	_panel->SetDrawBackground(false);
	_panel->SetDrawBorder(false);
	_panel->Move(0, -_panelFrame->GetHeight());
	_panelTitle = nullptr;
}

MainMenuDlg::~MainMenuDlg()
{
}

void MainMenuDlg::OnEditor()
{
	SwitchPanel(PT_EDITOR);
}

void MainMenuDlg::OnMapSettings()
{
//	SetVisible(false);
//	MapSettingsDlg *dlg = new MapSettingsDlg(GetParent(), _gameContext);
//	dlg->eventClose = std::bind(&MainMenuDlg::OnCloseChild, this, std::placeholders::_1);
}

void MainMenuDlg::OnImportMap()
{
	GetFileNameDlg::Params param;
	param.title = "Select map to load";
	param.folder = _fs.GetFileSystem(DIR_MAPS);
	param.extension = "map";

	if( !param.folder )
	{
		static_cast<Desktop *>(GetManager().GetDesktop())->ShowConsole(true);
		_logger.Printf(1, "Could not open directory '%s'", DIR_MAPS);
		return;
	}

	SetVisible(false);
	assert(nullptr == _fileDlg);
	_fileDlg = new GetFileNameDlg(GetParent(), param);
	_fileDlg->eventClose = std::bind(&MainMenuDlg::OnImportMapSelect, this, std::placeholders::_1);
}

void MainMenuDlg::OnImportMapSelect(int result)
{
	assert(_fileDlg);
	if(UI::Dialog::_resultOK == result )
	{
		_commands.openMap(std::string(DIR_MAPS) + "/" + _fileDlg->GetFileName());
	}
	_fileDlg = nullptr;
	OnCloseChild(result);
}

void MainMenuDlg::OnExportMap()
{
	GetFileNameDlg::Params param;
	param.title = "Select map to save map";
	param.folder = _fs.GetFileSystem(DIR_MAPS, true);
	param.extension = "map";

	if( !param.folder )
	{
		static_cast<Desktop *>(GetManager().GetDesktop())->ShowConsole(true);
		_logger.Printf(1, "ERROR: Could not open directory '%s'", DIR_MAPS);
		return;
	}

	SetVisible(false);
	assert(nullptr == _fileDlg);
	_fileDlg = new GetFileNameDlg(GetParent(), param);
	_fileDlg->eventClose = std::bind(&MainMenuDlg::OnExportMapSelect, this, std::placeholders::_1);
}

void MainMenuDlg::OnExportMapSelect(int result)
{
	assert(_fileDlg);
	if(UI::Dialog::_resultOK == result )
	{
		_commands.exportMap(std::string(DIR_MAPS) + "/" + _fileDlg->GetFileName());
	}
	_fileDlg = nullptr;
	OnCloseChild(result);
}

void MainMenuDlg::OnSettings()
{
	_commands.gameSettings();
}

void MainMenuDlg::OnCloseChild(int result)
{
	if(UI::Dialog::_resultOK == result )
	{
//		Close(result);
	}
	else
	{
		SetVisible(true);
		GetManager().SetFocusWnd(this);
	}
}

bool MainMenuDlg::OnKeyPressed(Key key)
{
	switch(key)
	{
	case Key::F12:
		OnSettings();
		break;
	case Key::Escape:
		if (_ptype != PT_NONE)
			SwitchPanel(PT_NONE);
		else
			return false;
		break;
	default:
		return false;
	}
	return true;
}

void MainMenuDlg::SwitchPanel(PanelType newtype)
{
	if( _ptype != newtype )
	{
		_ptype = newtype;
	}
	else
	{
		_ptype = PT_NONE;
	}
	_pstate = PS_DISAPPEARING;
	SetTimeStep(true);
}

void MainMenuDlg::CreatePanel()
{
	_panelTitle = UI::Text::Create(_panel, 0, 0, "", alignTextLT);
	_panelTitle->SetFont("font_default");

	float y = _panelTitle->GetCharHeight() + _panelTitle->GetY() + 10;
	UI::Button *btn;

	switch( _ptype )
	{
	case PT_EDITOR:
		_panelTitle->SetText("Editor");
		UI::Button::Create(_panel, "New", 0, y)->eventClick = _commands.newMap;
		UI::Button::Create(_panel, "Load", 100, y)->eventClick = std::bind(&MainMenuDlg::OnImportMap, this);
		btn = UI::Button::Create(_panel, "Save", 200, y);
		btn->eventClick = std::bind(&MainMenuDlg::OnExportMap, this);
		btn = UI::Button::Create(_panel, "Settings", 300, y);
		btn->eventClick = std::bind(&MainMenuDlg::OnMapSettings, this);
		break;
	default:
		assert(false);
	}
}

void MainMenuDlg::OnTimeStep(float dt)
{
	switch( _pstate )
	{
	case PS_APPEARING:
		_panel->Move(0, _panel->GetY() + dt * 1000);
		if( _panel->GetY() >= 0 )
		{
			_panel->Move(0, 0);
			_pstate = PS_NONE;
			SetTimeStep(false);
		}
		break;
	case PS_DISAPPEARING:
		_panel->Move(0, _panel->GetY() - dt * 1000);
		if( _panel->GetY() <= -_panelFrame->GetHeight() )
		{
			while( _panel->GetFirstChild() )
			{
				_panel->GetFirstChild()->Destroy();
			}
			_panel->Move(0, -_panelFrame->GetHeight());
			_panelTitle = nullptr;

			if( PT_NONE != _ptype )
			{
				_pstate = PS_APPEARING;
				CreatePanel();
			}
			else
			{
				_pstate = PS_NONE;
				SetTimeStep(false);
			}
		}
		break;
	default:
		assert(false);
	}
}

