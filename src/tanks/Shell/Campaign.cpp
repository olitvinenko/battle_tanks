#include "Campaign.h"
#include "Configuration.h"

#include "../FileSystem/FileSystem.h"
#include "../UI/List.h"
#include "../UI/Button.h"
#include "../UI/Text.h"
#include "../UI/DataSourceAdapters.h"
#include "../UI/GuiManager.h"


NewCampaignDlg::NewCampaignDlg(UI::UIWindow *parent, FileSystem::IFileSystem &fs)
  : UI::Dialog(parent, 512, 400)
  , _fs(fs)
{
    UI::Text *t = UI::Text::Create(this, GetWidth() / 2, 16, "New campaign", alignTextCT);
    t->SetFont("font_default");

    _files = DefaultListBox::Create(this);
    _files->Move(20, 56);
    _files->Resize(472, 280);
    
    _files->GetData()->AddItem("test1");
    _files->GetData()->AddItem("test2");
    _files->GetData()->AddItem("test3");

    _files->GetData()->Sort();

    UI::Button::Create(this, "OK", 290, 360)->eventClick = std::bind(&NewCampaignDlg::OnOK, this);
    UI::Button::Create(this, "Cancel", 400, 360)->eventClick = std::bind(&NewCampaignDlg::OnCancel, this);
}

NewCampaignDlg::~NewCampaignDlg()
{
}

void NewCampaignDlg::OnOK()
{
    if( -1 != _files->GetCurSel() )
    {
        if( eventCampaignSelected )
            eventCampaignSelected(_files->GetData()->GetItemText(_files->GetCurSel(), 0));
    }
}

void NewCampaignDlg::OnCancel()
{
    if( eventCampaignSelected )
        eventCampaignSelected(std::string());
}
