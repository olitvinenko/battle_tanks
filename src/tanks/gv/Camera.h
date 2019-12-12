#pragma once

#include "math/Rect.h"

class GC_Player;
class World;

class Camera
{
public:
	Camera(Vector2 pos, GC_Player &player);

	void CameraTimeStep(World &world, float dt, float scale);

	void SetViewport(RectInt viewport) { _viewport = viewport; }
	RectInt GetViewport() const { return _viewport; }

	Vector2 GetCameraPos() const;
	GC_Player& GetPlayer() const { return _player; }
	float GetShake() const { return _time_shake; }
	void Shake(float level);

private:
	GC_Player &_player;
	Vector2 _pos;
	Vector2 _target;
	float _time_shake;
	float _time_seed;
	RectInt _viewport;
};
