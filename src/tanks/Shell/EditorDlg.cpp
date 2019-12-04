#include "EditorDlg.h"
#include "ConfigBinding.h"
#include "loc/Language.h"
#include "ui/Button.h"
#include "ui/DataSource.h"
#include "ui/GuiManager.h"

static const float c_buttonWidth = 200;
static const float c_buttonHeight = 60;


EditorDlg::EditorDlg(UI::LayoutManager &manager,
                    TextureManager &texman,
                    LangCache &lang,
                    Commands&& commands)
  : StackLayout(manager)
  , _lang(lang)
  , _commands(std::move(commands))
{
	SetFlowDirection(UI::FlowDirection::Vertical);
	SetSpacing(10);

	Resize(c_buttonWidth, 640);

	std::shared_ptr<UI::Button> button;

	button = std::make_shared<UI::Button>(manager, texman);
	button->SetFont(texman, "font_default");
	button->SetText(ConfBind(_lang.editor_new_map));
	button->Resize(c_buttonWidth, c_buttonHeight);
    button->eventClick = _commands.newMap;
	AddFront(button);

	button = std::make_shared<UI::Button>(manager, texman);
	button->SetFont(texman, "font_default");
	button->SetText(ConfBind(_lang.editor_load_map));
	button->Resize(c_buttonWidth, c_buttonHeight);
	button->eventClick = _commands.openMap;
	AddFront(button);

	button = std::make_shared<UI::Button>(manager, texman);
	button->SetFont(texman, "font_default");
	button->SetText(ConfBind(_lang.editor_save_map));
	button->Resize(c_buttonWidth, c_buttonHeight);
	button->eventClick = _commands.saveMap;
	AddFront(button);
}
