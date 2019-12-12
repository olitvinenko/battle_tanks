#pragma once

#include "math/Rect.h"

namespace UI
{
	struct IInput;
}

class DefaultCamera
{
public:
	explicit DefaultCamera(Vector2 pos);

    void Move(Vector2 offset, const RectFloat &worldBounds);
	void HandleMovement(UI::IInput &input, const RectFloat &worldBounds);
	float GetZoom() const { return _zoom; }
	Vector2 GetEye() const { return _pos; }

private:
	float _zoom;
	float _dt;
	Vector2 _pos;
	unsigned int _dwTimeX;
	unsigned int _dwTimeY;
};
