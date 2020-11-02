#pragma once

namespace Internal
{
	struct BaseEventListener
	{
		virtual ~BaseEventListener() {};
	};
}

class ECSWorld;

template<typename T>
class EventListener : public Internal::BaseEventListener
{
public:
	virtual ~EventListener() {}

	virtual void Receive(ECSWorld* world, const T& event) = 0;
};
