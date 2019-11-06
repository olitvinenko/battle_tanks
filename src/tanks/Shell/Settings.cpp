#include "ConfigBinding.h"
#include "Settings.h"
#include "KeyMapper.h"
#include "Configuration.h"

#include <TextureManager.h>
#include <Language.h>
#include <Text.h>
#include <List.h>
#include <ListBox.h>
#include <ListSelectionBinding.h>
#include <MultiColumnListItem.h>
#include <Button.h>
#include <Scroll.h>
#include <Edit.h>
#include <Combo.h>
#include <DataSourceAdapters.h>
#include <GuiManager.h>
#include <Keys.h>

#include <algorithm>
#include <sstream>


SettingsDlg::SettingsDlg(UI::LayoutManager &manager, TextureManager &texman, ShellConfig &conf, LangCache &lang)
  : Dialog(manager, texman)
  , _conf(conf)
  , _lang(lang)
{
    Resize(512, 296);
    
	auto text = std::make_shared<UI::Text>(manager, texman);
	text->Move(GetWidth() / 2, 16);
	text->SetText(ConfBind(_lang.settings_title));
	text->SetAlign(alignTextCT);
	text->SetFont(texman, "font_default");
	AddFront(text);


	//
	// profiles
	//

	float x = 24;
	float y = 48;

	text = std::make_shared<UI::Text>(manager, texman);
	text->Move(x, y);
	text->SetText(ConfBind(_lang.settings_player1));
	AddFront(text);
	y += text->GetHeight() + 2;

	_player1 = std::make_shared<UI::ComboBox>(manager, texman, &_profilesDataSource);
	_player1->Move(x, y);
	_player1->Resize(128);
	_player1->GetList()->Resize(128, 52);
	AddFront(_player1);
	y += _player1->GetHeight() + 5;

	text = std::make_shared<UI::Text>(manager, texman);
	text->Move(24, y);
	text->SetText(ConfBind(_lang.settings_player2));
	AddFront(text);
	y += text->GetHeight() + 2;

	_player2 = std::make_shared<UI::ComboBox>(manager, texman, &_profilesDataSource);
	_player2->Move(x, y);
	_player2->Resize(128);
	_player2->GetList()->Resize(128, 52);
	AddFront(_player2);
	y += _player2->GetHeight() + 5;

	text = std::make_shared<UI::Text>(manager, texman);
	text->Move(x, y);
	text->SetText(ConfBind(_lang.settings_profiles));
	AddFront(text);
	y += text->GetHeight() + 2;

	_profiles = std::make_shared<UI::ListBox>(manager, texman, &_profilesDataSource);
	_profiles->Move(x, y);
	_profiles->Resize(128, 52);
	AddFront(_profiles);
	UpdateProfilesList(); // fill the list before binding OnChangeSel

	auto btn = std::make_shared<UI::Button>(manager, texman);
	btn->SetText(ConfBind(_lang.settings_profile_new));
	btn->Move(40, 184);
	btn->eventClick = std::bind(&SettingsDlg::OnAddProfile, this);
	AddFront(btn);

	_editProfile = std::make_shared<UI::Button>(manager, texman);
	_editProfile->SetText(ConfBind(_lang.settings_profile_edit));
	_editProfile->Move(40, 216);
	_editProfile->eventClick = std::bind(&SettingsDlg::OnEditProfile, this);
	_editProfile->SetEnabled(std::make_shared<UI::HasSelection>(_profiles->GetList()));
	AddFront(_editProfile);

	_deleteProfile = std::make_shared<UI::Button>(manager, texman);
	_deleteProfile->SetText(ConfBind(_lang.settings_profile_delete));
	_deleteProfile->Move(40, 248);
	_deleteProfile->eventClick = std::bind(&SettingsDlg::OnDeleteProfile, this);
	_deleteProfile->SetEnabled(std::make_shared<UI::HasSelection>(_profiles->GetList()));
	AddFront(_deleteProfile);


	//
	// other settings
	//

	x = 200;
	y = 48;

	_showFps = std::make_shared<UI::CheckBox>(manager, texman);
	_showFps->SetCheck(_conf.ui_showfps.Get());
	_showFps->Move(x, y);
	_showFps->SetText(texman, _lang.settings_show_fps.Get());
	AddFront(_showFps);
	y += _showFps->GetHeight();

	_showTime = std::make_shared<UI::CheckBox>(manager, texman);
	_showTime->SetCheck(_conf.ui_showtime.Get());
	_showTime->Move(x, y);
	_showTime->SetText(texman, _lang.settings_show_time.Get());
	AddFront(_showTime);
	y += _showTime->GetHeight();

	_showNames = std::make_shared<UI::CheckBox>(manager, texman);
	_showNames->SetCheck(_conf.g_shownames.Get());
	_showNames->Move(x, y);
	_showNames->SetText(texman, _lang.settings_show_names.Get());
	AddFront(_showNames);
	y += _showNames->GetHeight();

	text = std::make_shared<UI::Text>(manager, texman);
	text->Move(x + 50, y += 20);
	text->SetText(ConfBind(_lang.settings_sfx_volume));
	text->SetAlign(alignTextRT);
	AddFront(text);

	_volumeSfx = std::make_shared<UI::ScrollBarHorizontal>(manager, texman);
	_volumeSfx->Move(x + 60, y);
	_volumeSfx->SetWidth(150);
	_volumeSfx->SetDocumentSize(1);
	_volumeSfx->SetLineSize(0.1f);
	_volumeSfx->SetPos(expf(_conf.s_volume.GetFloat() / 2171.0f) - 0.01f);
	_volumeSfx->eventScroll = std::bind(&SettingsDlg::OnVolumeSfx, this, std::placeholders::_1);
	AddFront(_volumeSfx);
	_initialVolumeSfx = _conf.s_volume.GetInt();

	text = std::make_shared<UI::Text>(manager, texman);
	text->Move(x + 50, y += 20);
	text->SetText(ConfBind(_lang.settings_music_volume));
	text->SetAlign(alignTextRT);
	AddFront(text);

	_volumeMusic = std::make_shared<UI::ScrollBarHorizontal>(manager, texman);
	_volumeMusic->Move(x + 60, y);
	_volumeMusic->SetWidth(150);
	_volumeMusic->SetDocumentSize(1);
	_volumeMusic->SetLineSize(0.1f);
	_volumeMusic->SetPos(expf(_conf.s_musicvolume.GetFloat() / 2171.0f) - 0.01f);
	_volumeMusic->eventScroll = std::bind(&SettingsDlg::OnVolumeMusic, this, std::placeholders::_1);
	AddFront(_volumeMusic);
	_initialVolumeMusic = _conf.s_musicvolume.GetInt();


	//
	// OK & Cancel
	//

	btn = std::make_shared<UI::Button>(manager, texman);
	btn->SetText(ConfBind(_lang.common_ok));
	btn->Move(304, 256);
	btn->eventClick = std::bind(&SettingsDlg::OnOK, this);
	AddFront(btn);

	btn = std::make_shared<UI::Button>(manager, texman);
	btn->SetText(ConfBind(_lang.common_cancel));
	btn->Move(408, 256);
	btn->eventClick = std::bind(&SettingsDlg::OnCancel, this);
	AddFront(btn);

	_profiles->GetList()->SetCurSel(0, true);
	SetFocus(_profiles);
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::OnVolumeSfx(float pos)
{
	_conf.s_volume.SetInt( int(2171.0f * logf(0.01f + pos)) );
}

void SettingsDlg::OnVolumeMusic(float pos)
{
	_conf.s_musicvolume.SetInt( int(2171.0f * logf(0.01f + pos)) );
}

void SettingsDlg::OnAddProfile()
{
	auto dlg = std::make_shared<ControlProfileDlg>(GetManager(), GetManager().GetTextureManager(), nullptr, _conf, _lang);
	dlg->eventClose = std::bind(&SettingsDlg::OnProfileEditorClosed, this, std::placeholders::_1, std::placeholders::_2);
	AddFront(dlg);
	SetFocus(dlg);
}

void SettingsDlg::OnEditProfile()
{
	int i = _profiles->GetList()->GetCurSel();
	assert(i >= 0);
	auto dlg = std::make_shared<ControlProfileDlg>(GetManager(), GetManager().GetTextureManager(), _profilesDataSource.GetItemText(i, 0).c_str(), _conf, _lang);
	dlg->eventClose = std::bind(&SettingsDlg::OnProfileEditorClosed, this, std::placeholders::_1, std::placeholders::_2);
	AddFront(dlg);
	SetFocus(dlg);
}

void SettingsDlg::OnDeleteProfile()
{
	int i = _profiles->GetList()->GetCurSel();
	assert(i >= 0);
	if( _conf.cl_playerinfo.profile.Get() == _profilesDataSource.GetItemText(i, 0) )
	{
		// profile that is being deleted is used in network settings
		_conf.cl_playerinfo.profile.Set("");
	}
	_conf.dm_profiles.Remove(_profilesDataSource.GetItemText(i, 0));
	UpdateProfilesList();
}

void SettingsDlg::OnOK()
{
	_conf.ui_showfps.Set(_showFps->GetCheck());
	_conf.ui_showtime.Set(_showTime->GetCheck());
	_conf.g_shownames.Set(_showNames->GetCheck());

	Close(_resultCancel); // return cancel to show back the main menu
}

void SettingsDlg::OnCancel()
{
	_conf.s_volume.SetInt(_initialVolumeSfx);
	_conf.s_musicvolume.SetInt(_initialVolumeMusic);
	Close(_resultCancel);
}

void SettingsDlg::UpdateProfilesList()
{
	int sel = _profiles->GetList()->GetCurSel();
	std::vector<std::string> profiles = _conf.dm_profiles.GetKeys();
	_profilesDataSource.DeleteAllItems();
	for( size_t i = 0; i < profiles.size(); ++i )
	{
		_profilesDataSource.AddItem(profiles[i]);
	}
	_profiles->GetList()->SetCurSel(std::min(_profilesDataSource.GetItemCount() - 1, sel));
}

void SettingsDlg::OnProfileEditorClosed(std::shared_ptr<UI::Dialog> sender, int result)
{
	if( _resultOK == result )
	{
		UpdateProfilesList();
		SetFocus(_profiles);
	}
	UnlinkChild(*sender);
}

///////////////////////////////////////////////////////////////////////////////
// class ControlProfileDlg

static std::string GenerateProfileName(const ShellConfig &conf, LangCache &lang)
{
	int i = 0;
	std::ostringstream buf;
	do
	{
		buf.str("");
		buf << lang.profile_autoname.Get() << ++i;
	}
	while( conf.dm_profiles.Find(buf.str()) );
	return buf.str();
}

ControlProfileDlg::ControlProfileDlg(UI::LayoutManager &manager, TextureManager &texman, const char *profileName, ShellConfig &conf, LangCache &lang)
  : Dialog(manager, texman)
  , _nameOrig(profileName ? profileName : GenerateProfileName(conf, lang))
  , _profile(&conf.dm_profiles.GetTable(_nameOrig))
  , _conf(conf)
  , _lang(lang)
  , _time(0)
  , _activeIndex(-1)
  , _createNewProfile(!profileName)
{
    Resize(448, 416);
    
	auto text = std::make_shared<UI::Text>(manager, texman);
	text->Move(20, 15);
	text->SetText(ConfBind(_lang.profile_name));
	AddFront(text);

	_nameEdit = std::make_shared<UI::Edit>(manager, texman);
	_nameEdit->Move(20, 30);
	_nameEdit->SetWidth(250);
	_nameEdit->SetText(texman, _nameOrig);
	AddFront(_nameEdit);

	text = std::make_shared<UI::Text>(manager, texman);
	text->Move(20, 65);
	text->SetText(ConfBind(_lang.profile_action));
	AddFront(text);

	text = std::make_shared<UI::Text>(manager, texman);
	text->Move(220, 65);
	text->SetText(ConfBind(_lang.profile_key));
	AddFront(text);

	auto itemTemplate = std::make_shared<UI::MultiColumnListItem>(manager, texman);
	itemTemplate->EnsureColumn(manager, texman, 0, 2);
	itemTemplate->EnsureColumn(manager, texman, 1, 200);

	_actions = std::make_shared<DefaultListBox>(manager, texman);
	_actions->Move(20, 80);
	_actions->Resize(400, 250);
	_actions->GetList()->SetItemTemplate(itemTemplate);
	_actions->GetList()->eventClickItem = std::bind(&ControlProfileDlg::OnSelectAction, this, std::placeholders::_1);
	AddFront(_actions);

	AddAction(_profile.key_forward      , _lang.action_move_forward.Get()  );
	AddAction(_profile.key_back         , _lang.action_move_backward.Get() );
	AddAction(_profile.key_left         , _lang.action_turn_left.Get()     );
	AddAction(_profile.key_right        , _lang.action_turn_right.Get()    );
	AddAction(_profile.key_fire         , _lang.action_fire.Get()          );
	AddAction(_profile.key_light        , _lang.action_toggle_lights.Get() );
	AddAction(_profile.key_tower_left   , _lang.action_tower_left.Get()    );
	AddAction(_profile.key_tower_right  , _lang.action_tower_right.Get()   );
	AddAction(_profile.key_tower_center , _lang.action_tower_center.Get()  );
	AddAction(_profile.key_no_pickup    , _lang.action_no_pickup.Get()     );
	_actions->GetList()->SetCurSel(0, true);

	_aimToMouseChkBox = std::make_shared<UI::CheckBox>(manager, texman);
	_aimToMouseChkBox->SetCheck(_profile.aim_to_mouse.Get());
	_aimToMouseChkBox->Move(16, 345);
	_aimToMouseChkBox->SetText(texman, _lang.profile_mouse_aim.Get());
	AddFront(_aimToMouseChkBox);

	_moveToMouseChkBox = std::make_shared<UI::CheckBox>(manager, texman);
	_moveToMouseChkBox->SetCheck(_profile.move_to_mouse.Get());
	_moveToMouseChkBox->Move(146, 345);
	_moveToMouseChkBox->SetText(texman, _lang.profile_mouse_move.Get());
	AddFront(_moveToMouseChkBox);

	_arcadeStyleChkBox = std::make_shared<UI::CheckBox>(manager, texman);
	_arcadeStyleChkBox->SetCheck(_profile.arcade_style.Get());
	_arcadeStyleChkBox->Move(276, 345);
	_arcadeStyleChkBox->SetText(texman, _lang.profile_arcade_style.Get());
	AddFront(_arcadeStyleChkBox);

	auto btn = std::make_shared<UI::Button>(manager, texman);
	btn->SetText(ConfBind(_lang.common_ok));
	btn->Move(240, 380);
	btn->eventClick = std::bind(&ControlProfileDlg::OnOK, this);
	AddFront(btn);

	btn = std::make_shared<UI::Button>(manager, texman);
	btn->SetText(ConfBind(_lang.common_cancel));
	btn->Move(344, 380);
	btn->eventClick = std::bind(&ControlProfileDlg::OnCancel, this);
	AddFront(btn);

	SetFocus(_actions);
}

ControlProfileDlg::~ControlProfileDlg()
{
}

void ControlProfileDlg::OnSelectAction(int index)
{
	SetActiveIndex(index);
	SetFocus(nullptr);
}

void ControlProfileDlg::SetActiveIndex(int index)
{
	if (index == _activeIndex)
		return;

	if (_activeIndex != -1)
	{
		_actions->GetData()->SetItemText(_activeIndex, 1, GetKeyName(_keyBindings[_activeIndex]));
	}

	_activeIndex = index;

	if (_activeIndex != -1)
	{
		_actions->GetData()->SetItemText(_activeIndex, 1, "...");
		SetTimeStep(true);
		_time = 0;
	}
	else
	{
		SetTimeStep(false);
	}
}

void ControlProfileDlg::AddAction(ConfVarString &keyName, std::string actionDisplayName)
{
	_keyBindings.push_back(GetKeyCode(keyName.Get()));
	int index = _actions->GetData()->AddItem(std::move(actionDisplayName), reinterpret_cast<size_t>(&keyName));
	_actions->GetData()->SetItemText(index, 1, GetKeyName(_keyBindings.back()));
	assert(_actions->GetData()->GetItemCount() == _keyBindings.size());
}

void ControlProfileDlg::OnOK()
{
	if( _nameEdit->GetText().empty() || !_conf.dm_profiles.Rename(_profile, _nameEdit->GetText()) )
	{
		return;
	}

	for( int i = 0; i < _actions->GetData()->GetItemCount(); ++i )
	{
		reinterpret_cast<ConfVarString*>(_actions->GetData()->GetItemData(i))->Set(GetKeyName(_keyBindings[i]));
	}

	_profile.aim_to_mouse.Set(_aimToMouseChkBox->GetCheck());
	_profile.move_to_mouse.Set(_moveToMouseChkBox->GetCheck());
	_profile.arcade_style.Set(_arcadeStyleChkBox->GetCheck());

	Close(_resultOK);
}

void ControlProfileDlg::OnCancel()
{
	if( _createNewProfile )
	{
		_conf.dm_profiles.Remove(_profile);
	}
	Close(_resultCancel);
}

void ControlProfileDlg::OnTimeStep(UI::LayoutManager &manager, float dt)
{
	_time += dt;
	_actions->GetData()->SetItemText(_activeIndex, 1, fmodf(_time, 0.6f) > 0.3f ? "" : "...");
}

bool ControlProfileDlg::OnKeyPressed(UI::InputContext &ic, UI::Key key)
{
	if (-1 != _activeIndex)
	{
		if (UI::Key::Escape != key)
		{
			_keyBindings[_activeIndex] = key;
		}
		SetActiveIndex(-1);
		SetFocus(_actions);
	}
	else
	{
		switch( key )
		{
		case UI::Key::Enter:
			if( GetFocus() == _actions && -1 != _actions->GetList()->GetCurSel() )
			{
				OnSelectAction(_actions->GetList()->GetCurSel());
			}
			else
			{
				OnOK();
			}
			break;
		case UI::Key::Escape:
			break;
		default:
			return Dialog::OnKeyPressed(ic, key);
		}
	}
	return true;
}
