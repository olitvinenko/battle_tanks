#pragma once

namespace Internal
{
	struct BaseEventSubscriber
	{
		virtual ~BaseEventSubscriber() {};
	};
}

class World;

/**
 * Subclass this as EventSubscriber<EventType> and then call World::subscribe() in order to subscribe to events. Make sure
 * to call World::unsubscribe() or World::unsubscribeAll() when your subscriber is deleted!
 */
template<typename T>
class EventSubscriber : public Internal::BaseEventSubscriber
{
public:
	virtual ~EventSubscriber() {}

	/**
	 * Called when an event is emitted by the world.
	 */
	virtual void receive(World* world, const T& event) = 0;
};
