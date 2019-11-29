#pragma once

#include "ui/Dialog.h"

class LangCache;
class TextureManager;

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ListDataSourceDefault;
	class List;
	template <class, class> class ListAdapter;
}

class NewCampaignDlg : public UI::Dialog
{
public:
	NewCampaignDlg(UI::LayoutManager &manager, TextureManager &texman, FileSystem::IFileSystem &fs, LangCache &lang);
	~NewCampaignDlg();
	std::function<void(std::shared_ptr<NewCampaignDlg>, std::string)> eventCampaignSelected;

private:
	void OnOK();
	void OnCancel();

	typedef UI::ListAdapter<UI::ListDataSourceDefault, UI::List> DefaultListBox;
	std::shared_ptr<DefaultListBox> _files;
	FileSystem::IFileSystem &_fs;
};
