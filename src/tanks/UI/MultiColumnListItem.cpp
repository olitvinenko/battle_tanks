#include "DataSource.h"
#include "ListBase.h"
#include "LayoutContext.h"
#include "MultiColumnListItem.h"
#include "Rectangle.h"
#include "StateContext.h"
#include "Text.h"

using namespace UI;

MultiColumnListItem::MultiColumnListItem(LayoutManager &manager, TextureManager &texman)
	: Window(manager)
	, _selection(std::make_shared<Rectangle>(manager))
{
	static const auto selectionFillColorMap = std::make_shared<UI::ColorMap>(0x00000000, // default
		UI::ColorMap::ColorMapType{ { "Focused", 0xffffffff } });
	static const auto selectionBorderColorMap = std::make_shared<UI::ColorMap>(0x00000000, // default
		UI::ColorMap::ColorMapType{ { "Focused", 0xffffffff },{ "Unfocused", 0xffffffff } });

	AddBack(_selection);
	_selection->SetTexture(texman, "ui/listsel", false);
	_selection->SetBackColor(selectionFillColorMap);
	_selection->SetBorderColor(selectionBorderColorMap);

	EnsureColumn(manager, texman, 0u, 0.f);
}

void MultiColumnListItem::EnsureColumn(LayoutManager &manager, TextureManager &texman, unsigned int columnIndex, float offset)
{
	static const auto textColorMap = std::make_shared<UI::ColorMap>(0xffffffff, // default
		UI::ColorMap::ColorMapType{ { "Disabled", 0xbbbbbbbb },{ "Hover", 0xffccccff },{ "Focused", 0xff000000 } });

	if (columnIndex >= _columns.size())
		_columns.insert(_columns.end(), 1 + columnIndex - _columns.size(), nullptr);

	if (!_columns[columnIndex])
	{
		// TODO: reuse the text object
		_columns[columnIndex] = std::make_shared<UI::Text>(manager, texman);
		AddFront(_columns[columnIndex]);
	}

	_columns[columnIndex]->Move(offset, 0);
	_columns[columnIndex]->SetText(std::make_shared<ListDataSourceBinding>(columnIndex));
	_columns[columnIndex]->SetFont(texman, "font_small");
	_columns[columnIndex]->SetFontColor(textColorMap);
}

Vector2 MultiColumnListItem::GetContentSize(TextureManager &texman, const StateContext &sc, float scale) const
{
	return _columns[0]->GetContentSize(texman, sc, scale);
}

RectFloat MultiColumnListItem::GetChildRect(TextureManager &texman, const LayoutContext &lc, const StateContext &sc, const Window &child) const
{
	float scale = lc.GetScale();
	Vector2 size = lc.GetPixelSize();

	if (_selection.get() == &child)
	{
		Vector2 pxMargins = Vec2dFloor(Vector2{ 1, 2 } * scale);
		return MakeRectWH(-pxMargins, size + pxMargins * 2);
	}

	return Window::GetChildRect(texman, lc, sc, child);
}

