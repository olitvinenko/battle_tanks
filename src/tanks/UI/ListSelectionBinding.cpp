#include "ListSelectionBinding.h"
#include "List.h"

using namespace UI;

HasSelection::HasSelection(std::weak_ptr<UI::List> list)
	: _list(std::move(list))
{}

bool HasSelection::GetValue(const UI::StateContext &sc) const
{
	if (auto list = _list.lock())
	{
		return list->GetCurSel() != -1;
	}
	return false;
}
