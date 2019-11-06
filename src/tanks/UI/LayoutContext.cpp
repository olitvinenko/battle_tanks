#include "LayoutContext.h"
#include "Window.h"

using namespace UI;

LayoutContext::LayoutContext(float opacity, float scale, vec2d offset, vec2d size, bool enabled)
	: _offset(offset)
	, _size(size)
	, _scale(scale)
	, _opacityCombined(opacity)
	, _enabled(enabled)
{
}

LayoutContext::LayoutContext(TextureManager &texman, const Window &parentWindow, const LayoutContext &parentLC, const StateContext &parentSC, const Window &childWindow, const StateContext &childSC)
	: _scale(parentLC.GetScale())
	, _opacityCombined(parentLC.GetOpacityCombined() * parentWindow.GetChildOpacity(childWindow))
	, _enabled(parentLC.GetEnabledCombined() && childWindow.GetEnabled(childSC))
{
	auto childRect = parentWindow.GetChildRect(texman, parentLC, parentSC, childWindow);
	_offset = parentLC.GetPixelOffset() + Offset(childRect);
	_size = Size(childRect);
}
