#pragma once

class World;

struct GameContextBase
{
	virtual ~GameContextBase() = default;

	virtual World& GetWorld() = 0;
	virtual void FixedUpdate(float dt) = 0;
};
