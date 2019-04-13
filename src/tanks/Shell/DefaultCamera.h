// DefaultCamera.h

#pragma once
#include "Vector2.h"

struct IInput;

class DefaultCamera
{
public:
	DefaultCamera();

	void HandleMovement(IInput &input, float worldWidth, float worldHeight, float screenWidth, float screenHeight);
	float GetZoom() const { return _zoom; }
	Vector2 GetPos() const { return _pos; }

private:
	float _zoom;
	float _dt;
	Vector2 _pos;
	unsigned int _dwTimeX;
	unsigned int _dwTimeY;
};

// end of file
