#pragma once
#include "Raven_Game.h"

struct GameContextBase
{
	virtual ~GameContextBase() = default;

	virtual Raven_Game& GetGame() = 0;

	virtual void FixedUpdate(float dt) = 0;
};
