#pragma once
#include "Dialog.h"

namespace UI
{
	class Edit;
}

class NewMapDlg : public UI::Dialog
{
	UI::Edit *_width;
	UI::Edit *_height;

public:
	std::function<void(int, int)> onOkCallback;

	NewMapDlg(UIWindow *parent);

	void OnOK();
	void OnCancel();
};
