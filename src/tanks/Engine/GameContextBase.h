#pragma once

class Pathfinder;

struct GameContextBase
{
	virtual ~GameContextBase() = default;

	virtual Pathfinder& GetPathfinder() = 0;

	virtual void FixedUpdate(float dt) = 0;
};
