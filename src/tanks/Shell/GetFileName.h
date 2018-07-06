#pragma once
#include "../ui/Dialog.h"
#include <string>
#include <memory>

namespace FileSystem
{
	class IFileSystem;
}

namespace UI
{
	class ListDataSourceDefault;
	class List;
	class Edit;
	template<class, class> class ListAdapter;
}

class GetFileNameDlg : public UI::Dialog
{
	typedef UI::ListAdapter<UI::ListDataSourceDefault, UI::List> DefaultListBox;
	DefaultListBox *_files;
	UI::Edit *_fileName;
	std::string _ext;
	std::shared_ptr<FileSystem::IFileSystem> _folder;

	bool _changing;

public:
	struct Params
	{
		std::string title;
		std::string extension;
		std::shared_ptr<FileSystem::IFileSystem> folder;
	};

	GetFileNameDlg(UIWindow *parent, const Params &param);
	virtual ~GetFileNameDlg();

	std::string GetFileName() const;
	std::string GetFileTitle() const;

protected:
	void OnSelect(int index);
	void OnChangeName();
	bool OnKeyPressed(UI::Key key);

	void OnOK();
	void OnCancel();
};
