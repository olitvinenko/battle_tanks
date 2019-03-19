#pragma once

class World;

/**
	 * A system that acts on entities. Generally, this will act on a subset of entities using World::each().
	 * Systems often will respond to events by subclassing EventSubscriber. You may use configure() to subscribe to events,
	 * but remember to unsubscribe in unconfigure().
	 */
struct EntitySystem
{
	virtual ~EntitySystem() {}

	/**
	 * Called when this system is added to a world.
	 */
	virtual void configure(World* world)
	{
	}

	/**
	 * Called when this system is being removed from a world.
	 */
	virtual void unconfigure(World* world)
	{
	}

	/**
	 * Called when World::tick() is called. See ECS_TICK_TYPE at the top of this file for more
	 * information about passing data to tick.
	 */
	virtual void tick(World* world, float data)
	{
	}
};