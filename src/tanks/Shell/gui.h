#pragma once
#include "Configuration.h"
#include <Dialog.h>

class LangCache;
class TextureManager;
namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class Button;
	class CheckBox;
	class ComboBox;
	class ConsoleBuffer;
	class Edit;
	class ListBox;
	class ListDataSourceDefault;
	class Text;
	template<class, class> class ListAdapter;
}
class ListDataSourceMaps;

class NewGameDlg : public UI::Dialog
{
	typedef UI::ListAdapter<ListDataSourceMaps, UI::ListBox> MapList;
	typedef UI::ListAdapter<UI::ListDataSourceDefault, UI::ListBox> DefaultListBox;

	TextureManager &_texman;
	ShellConfig &_conf;
	LangCache &_lang;
	std::shared_ptr<MapList> _maps;
	std::shared_ptr<DefaultListBox> _players;
	std::shared_ptr<DefaultListBox> _bots;
	std::shared_ptr<UI::CheckBox> _nightMode;
	std::shared_ptr<UI::Edit> _gameSpeed;
	std::shared_ptr<UI::Edit> _fragLimit;
	std::shared_ptr<UI::Edit> _timeLimit;

	std::shared_ptr<UI::Button> _removePlayer;
	std::shared_ptr<UI::Button> _changePlayer;
	std::shared_ptr<UI::Button> _removeBot;
	std::shared_ptr<UI::Button> _changeBot;

	bool _newPlayer;

public:
	NewGameDlg(UI::LayoutManager &manager, TextureManager &texman, FileSystem::IFileSystem &fs, ShellConfig &conf, UI::ConsoleBuffer &logger, LangCache &lang);
	~NewGameDlg() override;

    std::string GetSelectedMapName();
    
	bool OnKeyPressed(UI::InputContext &ic, UI::Key key) override;

protected:
	void RefreshPlayersList();
	void RefreshBotsList();

protected:
	void OnAddPlayer();
	void OnAddPlayerClose(std::shared_ptr<UI::Dialog> sender, int result);
	void OnEditPlayer();
	void OnEditPlayerClose(std::shared_ptr<UI::Dialog> sender, int result);
	void OnRemovePlayer();

	void OnAddBot();
	void OnAddBotClose(std::shared_ptr<UI::Dialog> sender, int result);
	void OnEditBot();
	void OnEditBotClose(std::shared_ptr<UI::Dialog> sender, int result);
	void OnRemoveBot();

	void OnOK();
	void OnCancel();
};

///////////////////////////////////////////////////////////////////////////////

class EditPlayerDlg : public UI::Dialog
{
	typedef UI::ListAdapter<UI::ListDataSourceDefault, UI::ComboBox> DefaultComboBox;

	std::shared_ptr<UI::Rectangle> _skinPreview;
	std::shared_ptr<UI::Edit> _name;
	std::shared_ptr<DefaultComboBox> _profiles;
	std::shared_ptr<DefaultComboBox> _skins;
	std::shared_ptr<DefaultComboBox> _classes;
	std::shared_ptr<DefaultComboBox> _teams;

	std::vector<std::pair<std::string, std::string>> _classNames;

	ConfPlayerLocal _info;

public:
	EditPlayerDlg(UI::LayoutManager &manager, TextureManager &texman, VariableTable &info, ShellConfig &conf, LangCache &lang);

protected:
	void OnChangeSkin(int index);

	// Dialog
	bool OnClose(int result) override;
};

///////////////////////////////////////////////////////////////////////////////

class EditBotDlg : public UI::Dialog
{
	typedef UI::ListAdapter<UI::ListDataSourceDefault, UI::ComboBox> DefaultComboBox;

	std::shared_ptr<UI::Edit> _name;
	std::shared_ptr<UI::Rectangle> _skinPreview;
	std::shared_ptr<DefaultComboBox> _skins;
	std::shared_ptr<DefaultComboBox> _classes;
	std::shared_ptr<DefaultComboBox> _teams;
	std::shared_ptr<DefaultComboBox> _levels;

	std::vector<std::pair<std::string, std::string>> _classNames;

	ConfPlayerAI _info;

public:
	EditBotDlg(UI::LayoutManager &manager, TextureManager &texman, VariableTable &info, LangCache &lang);

protected:
	void OnOK();
	void OnCancel();

	void OnChangeSkin(int index);
};
