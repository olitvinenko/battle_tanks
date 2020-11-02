#pragma once

class ECSWorld;

struct EntitySystem
{
	virtual ~EntitySystem() {}

	virtual void Configure(ECSWorld* world)
	{
	}

	virtual void Unconfigure(ECSWorld* world)
	{
	}

	virtual void Tick(ECSWorld* world, float data)
	{
	}
};
