#pragma once
#include "Pathfinder.h"

struct GameContextBase
{
	virtual ~GameContextBase() = default;

	//virtual Pathfinder& GetPathfinder() = 0;

	virtual void FixedUpdate(float dt) = 0;
};
