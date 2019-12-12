#include "LayoutContext.h"
#include "ListBox.h"
#include "List.h"
#include "MultiColumnListItem.h"
#include "Rectangle.h"
#include "ScrollView.h"

using namespace UI;

static const Vector2 c_borderSize = { 1.f, 1.f };

ListBox::ListBox(LayoutManager &manager, TextureManager &texman, ListDataSource* dataSource)
	: Window(manager)
	, _background(std::make_shared<Rectangle>(manager))
	, _scrollView(std::make_shared<ScrollView>(manager))
	, _list(std::make_shared<List>(manager, texman, dataSource))
{
	AddFront(_background);
	AddFront(_scrollView);

	_list->SetItemTemplate(std::make_shared<UI::MultiColumnListItem>(manager, texman));

	_scrollView->SetContent(_list);
	SetFocus(_scrollView);

	_background->SetTexture(texman, "ui/list", false);
	_background->SetDrawBorder(true);
}

RectFloat ListBox::GetChildRect(TextureManager &texman, const LayoutContext &lc, const StateContext &sc, const Window &child) const
{
	float scale = lc.GetScale();
	Vector2 size = lc.GetPixelSize();

	if (_background.get() == &child)
	{
		return MakeRectWH(size);
	}
	else if (_scrollView.get() == &child)
	{
		Vector2 pxBorderSize = Vec2dFloor(c_borderSize * scale);
		return MakeRectRB(pxBorderSize, size - pxBorderSize);
	}

	return Window::GetChildRect(texman, lc, sc, child);
}

Vector2 ListBox::GetContentSize(TextureManager &texman, const StateContext &sc, float scale) const
{
	return _scrollView->GetContentSize(texman, sc, scale) + Vec2dFloor(c_borderSize * scale) * 2;
}

