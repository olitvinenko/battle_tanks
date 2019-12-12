#pragma once

#include "math/Vector2.h"

namespace UI
{
	enum class Key;

	struct IInput
	{
		virtual bool IsKeyPressed(Key key) const = 0;
		virtual bool IsMousePressed(int button) const = 0;
		virtual Vector2 GetMousePos() const = 0;
	};
}
