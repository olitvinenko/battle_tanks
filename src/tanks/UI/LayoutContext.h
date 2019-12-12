#pragma once
#include "math/Vector2.h"
#include <vector>

class TextureManager;

namespace UI
{
	class Window;
	class StateContext;

	class LayoutContext
	{
	public:
		LayoutContext(float opacity, float scale, Vector2 offset, Vector2 size, bool enabled);
		LayoutContext(TextureManager &texman, const Window &parentWindow, const LayoutContext &parentLC, const StateContext &parentSC, const Window &childWindow, const StateContext &childSC);

		bool GetEnabledCombined() const { return _enabled; }
		Vector2 GetPixelOffset() const { return _offset; }
		Vector2 GetPixelSize() const { return _size; }
		float GetScale() const { return _scale; }
		float GetOpacityCombined() const { return _opacityCombined; }

	private:
		Vector2 _offset;
		Vector2 _size;
		float _scale;
		float _opacityCombined;
		bool _enabled;
	};

	inline float ToPx(float units, const LayoutContext& lc)
	{
		return std::floor(units * lc.GetScale());
	}

	inline Vector2 ToPx(Vector2 units, const LayoutContext& lc)
	{
		return Vec2dFloor(units * lc.GetScale());
	}

	inline Vector2 ToPx(Vector2 units, float scale)
	{
		return Vec2dFloor(units * scale);
	}
}
