#include "StackLayout.h"
#include "LayoutContext.h"
#include <algorithm>
using namespace UI;

StackLayout::StackLayout(LayoutManager &manager)
	: Window(manager)
{
}

RectFloat StackLayout::GetChildRect(TextureManager &texman, const LayoutContext &lc, const StateContext &sc, const Window &child) const
{
	float scale = lc.GetScale();
	Vector2 size = lc.GetPixelSize();

	// FIXME: O(n^2) complexity
	float pxOffset = 0;
	float pxSpacing = std::floor(_spacing * scale);
	auto &children = GetChildren();
	if (FlowDirection::Vertical == _flowDirection)
	{
		for (auto &item : children)
		{
			if (item.get() == &child)
			{
				break;
			}
			pxOffset += pxSpacing + item->GetContentSize(texman, sc, scale).y;
		}
		Vector2 pxChildSize = child.GetContentSize(texman, sc, scale);
		if (_align == Align::LT)
		{
			return RectFloat{ 0.f, pxOffset, size.x, pxOffset + pxChildSize.y };
		}
		else
		{
			assert(_align == Align::CT); // TODO: support others
			float pxMargin = std::floor(size.x - pxChildSize.x) / 2;
			return RectFloat{ pxMargin, pxOffset, size.x - pxMargin, pxOffset + pxChildSize.y };
		}
	}
	else
	{
		assert(FlowDirection::Horizontal == _flowDirection);
		for (auto &item : children)
		{
			if (item.get() == &child)
			{
				break;
			}
			pxOffset += pxSpacing + item->GetContentSize(texman, sc, scale).x;
		}
		return RectFloat{ pxOffset, 0.f, pxOffset + child.GetContentSize(texman, sc, scale).x, size.y };
	}
}

Vector2 StackLayout::GetContentSize(TextureManager &texman, const StateContext &sc, float scale) const
{
	float pxTotalSize = 0; // in flow direction
	unsigned int sumComponent = FlowDirection::Vertical == _flowDirection;

	float pxMaxSize = 0;
	unsigned int maxComponent = FlowDirection::Horizontal == _flowDirection;

	auto &children = GetChildren();
	for (auto &item : children)
	{
		Vector2 pxItemSize = item->GetContentSize(texman, sc, scale);
		pxTotalSize += pxItemSize[sumComponent];
		pxMaxSize = std::max(pxMaxSize, pxItemSize[maxComponent]);
	}

	if (children.size() > 1)
	{
		pxTotalSize += std::floor(_spacing * scale) * (float)(children.size() - 1);
	}

	return FlowDirection::Horizontal == _flowDirection ?
		Vector2{ pxTotalSize, pxMaxSize } :
		Vector2{ pxMaxSize, pxTotalSize };
}

