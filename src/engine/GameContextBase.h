#pragma once

#include <memory>

class Pathfinder;
class World;

struct GameContextBase
{
	virtual ~GameContextBase() = default;

	virtual std::shared_ptr<Pathfinder> GetPathfinder() = 0;
    virtual World& GetWorld() = 0;

	virtual void FixedUpdate(float dt) = 0;
};
