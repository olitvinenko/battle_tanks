#include "NewMap.h"
#include "Button.h"
#include "Edit.h"
#include "GuiManager.h"
#include "Text.h"
#include <algorithm>
#include "WorldCfg.h"

NewMapDlg::NewMapDlg(UIWindow *parent)
	: Dialog(parent, 256, 256)
{
	UI::Text *header = UI::Text::Create(this, 128, 20, "newmap_title", alignTextCT);
	header->SetFont("font_default");

	const int Def_width = 24;
	const int Def_height = 32;

	UI::Text::Create(this, 40, 75, "Width", alignTextLT);
	_width = UI::Edit::Create(this, 60, 90, 80);
	_width->SetInt(Def_width);

	UI::Text::Create(this, 40, 115, "Height", alignTextLT);
	_height = UI::Edit::Create(this, 60, 130, 80);
	_height->SetInt(Def_height);

	UI::Button::Create(this, "OK", 20, 200)->eventClick = std::bind(&NewMapDlg::OnOK, this);
	UI::Button::Create(this, "Cancel", 140, 200)->eventClick = std::bind(&NewMapDlg::OnCancel, this);

	GetManager().SetFocusWnd(_width);
}

void NewMapDlg::OnOK()
{
	onOkCallback(std::max(LEVEL_MINSIZE, std::min(LEVEL_MAXSIZE, _width->GetInt())), std::max(LEVEL_MINSIZE, std::min(LEVEL_MAXSIZE, _height->GetInt())));
	Close(_resultOK);
}

void NewMapDlg::OnCancel()
{
	Close(_resultCancel);
}
