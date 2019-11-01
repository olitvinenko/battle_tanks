#pragma once
#include "../UI/Dialog.h"

class TextureManager;

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ListDataSourceDefault;
	class List;
    class UIWindow;

	template <class, class> class ListAdapter;
}

class NewCampaignDlg : public UI::Dialog
{
    typedef UI::ListAdapter<UI::ListDataSourceDefault, UI::List> DefaultListBox;
    
public:
	NewCampaignDlg(UI::UIWindow* parent, FileSystem::IFileSystem &fs);
	~NewCampaignDlg();

    std::function<void(std::string)> eventCampaignSelected;

private:
	void OnOK();
	void OnCancel();

	DefaultListBox *_files;
	FileSystem::IFileSystem& _fs;
};
