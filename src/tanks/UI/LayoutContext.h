#pragma once
#include "math/MyMath.h"
#include <vector>

class TextureManager;

namespace UI
{
	class Window;
	class StateContext;

	class LayoutContext
	{
	public:
		LayoutContext(float opacity, float scale, vec2d offset, vec2d size, bool enabled);
		LayoutContext(TextureManager &texman, const Window &parentWindow, const LayoutContext &parentLC, const StateContext &parentSC, const Window &childWindow, const StateContext &childSC);

		bool GetEnabledCombined() const { return _enabled; }
		vec2d GetPixelOffset() const { return _offset; }
		vec2d GetPixelSize() const { return _size; }
		float GetScale() const { return _scale; }
		float GetOpacityCombined() const { return _opacityCombined; }

	private:
		vec2d _offset;
		vec2d _size;
		float _scale;
		float _opacityCombined;
		bool _enabled;
	};

	inline float ToPx(float units, const LayoutContext& lc)
	{
		return std::floor(units * lc.GetScale());
	}

	inline vec2d ToPx(vec2d units, const LayoutContext& lc)
	{
		return Vec2dFloor(units * lc.GetScale());
	}

	inline vec2d ToPx(vec2d units, float scale)
	{
		return Vec2dFloor(units * scale);
	}
}
