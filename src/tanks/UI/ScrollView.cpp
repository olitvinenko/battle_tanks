#include "LayoutContext.h"
#include "ScrollView.h"
#include <algorithm>
using namespace UI;

ScrollView::ScrollView(LayoutManager &manager)
	: Window(manager)
{
	SetClipChildren(true);
}

void ScrollView::SetContent(std::shared_ptr<Window> content)
{
	if (_content != content)
	{
		if (_content)
		{
			UnlinkChild(*_content);
			_offset = Vector2{};
		}
		_content = content;
		AddBack(_content);
		SetFocus(_content);
	}
}

RectFloat ScrollView::GetChildRect(TextureManager &texman, const LayoutContext &lc, const StateContext &sc, const Window &child) const
{
	float scale = lc.GetScale();
	Vector2 size = lc.GetPixelSize();

	if (_content.get() == &child)
	{
		Vector2 pxContentMeasuredSize = _content->GetContentSize(texman, sc, scale);
		Vector2 pxContentOffset = Vec2dConstrain(Vec2dFloor(_offset * scale), MakeRectWH(pxContentMeasuredSize - size));
		Vector2 pxContentSize = Vector2{
			_horizontalScrollEnabled ? pxContentMeasuredSize.x : size.x,
			_verticalScrollEnabled ? pxContentMeasuredSize.y : size.y };
		return MakeRectWH(-pxContentOffset, pxContentSize);
	}

	return Window::GetChildRect(texman, lc, sc, child);
}

Vector2 ScrollView::GetContentSize(TextureManager &texman, const StateContext &sc, float scale) const
{
	return _content ? _content->GetContentSize(texman, sc, scale) : Vector2{};
}

void ScrollView::OnScroll(TextureManager &texman, const UI::InputContext &ic, const UI::LayoutContext &lc, const UI::StateContext &sc, Vector2 pointerPosition, Vector2 scrollOffset)
{
	if (_content)
	{
		if (!_verticalScrollEnabled && _horizontalScrollEnabled && scrollOffset.x == 0)
		{
			std::swap(scrollOffset.x, scrollOffset.y);
		}

		Vector2 pxContentMeasuredSize = _content->GetContentSize(texman, sc, lc.GetScale());

		RectFloat offsetConstraints = MakeRectWH((pxContentMeasuredSize - lc.GetPixelSize()) / lc.GetScale());
		_offset = Vec2dConstrain(_offset, offsetConstraints);
		_offset -= scrollOffset * 30;
		_offset = Vec2dConstrain(_offset, offsetConstraints);
	}
	else
	{
		_offset = Vector2{};
	}
}
