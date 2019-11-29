#pragma once

#include "math/MyMath.h"

namespace UI
{
	struct IInput;
}

class DefaultCamera
{
public:
	explicit DefaultCamera(vec2d pos);

    void Move(vec2d offset, const FRECT &worldBounds);
	void HandleMovement(UI::IInput &input, const FRECT &worldBounds);
	float GetZoom() const { return _zoom; }
	vec2d GetEye() const { return _pos; }

private:
	float _zoom;
	float _dt;
	vec2d _pos;
	unsigned int _dwTimeX;
	unsigned int _dwTimeY;
};
