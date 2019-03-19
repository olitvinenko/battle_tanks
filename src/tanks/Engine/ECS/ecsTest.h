#include <iostream>
#include <numeric>
#include <thread>

#include "ECS.h"

using namespace std;

#define LINE(s) cout << s << endl

struct Position
{
	Position(float x, float y) : x(x), y(y) {}
	Position() : x(0.f), y(0.f) {}

	float x;
	float y;

	ECS_DECLARE_TYPE;
};

ECS_DEFINE_TYPE(Position);

struct Rotation
{
	Rotation(float angle) : angle(angle) {}
	Rotation() : angle(0) {}
	float angle;

	ECS_DECLARE_TYPE;
};

ECS_DEFINE_TYPE(Rotation);

struct MyEvent
{
	int foo;
	float bar;

	ECS_DECLARE_TYPE;
};

ECS_DEFINE_TYPE(MyEvent);

class GravitySystem
	: public EntitySystem
	, public EventSubscriber<MyEvent>
	, public EventSubscriber<OnComponentAssigned<Position>>
	, public EventSubscriber<OnComponentRemoved<Position>>
	, public EventSubscriber<OnEntityCreated>
	, public EventSubscriber<OnEntityDestroyed>
{
public:
	explicit GravitySystem(float amount)
	{
		gravityAmount = amount;
	}

	void configure(World* world) override
	{
		world->subscribe<OnComponentAssigned<Position>>(this);
		world->subscribe<OnComponentRemoved<Position>>(this);
		world->subscribe<OnEntityCreated>(this);
		world->subscribe<OnEntityDestroyed>(this);
		world->subscribe<MyEvent>(this);
	}

	void unconfigure(World* world) override
	{
		world->unsubscribe<OnComponentAssigned<Position>>(this);
		world->unsubscribe<OnComponentRemoved<Position>>(this);
		world->unsubscribe<OnEntityCreated>(this);
		world->unsubscribe<OnEntityDestroyed>(this);
		world->unsubscribe<MyEvent>(this);
	}

	void receive(World* world, const MyEvent& event) override
	{
		cout << "receive::MyEvent" << endl;
	}

	void receive(World* world, const OnComponentAssigned<Position>& event) override
	{
		cout << "receive::OnComponentAssigned" << endl;
	}

	void receive(World* world, const OnComponentRemoved<Position>& event) override
	{
		cout << "receive::OnComponentRemoved" << endl;
	}

	void receive(World* world, const OnEntityCreated& event) override
	{
		cout << "receive::OnEntityCreated" << endl;
	}

	void receive(World* world, const OnEntityDestroyed& event) override
	{
		cout << "receive::OnEntityDestroyed" << endl;
	}

	void tick(World* world, float deltaTime) override
	{
		world->each<Position>([&](Entity* ent, ComponentHandle<Position> position) {
			position->y += gravityAmount * deltaTime;
		});

		world->emit<MyEvent>({ 123, 45.67f }); // you can use initializer syntax if you want, this sets foo = 123 and bar = 45.67f
	}

private:
	float gravityAmount;
};

//TODO:: a.litvinenko: for testing only
void ECSTest()
{
	World* world = World::createWorld();
	world->registerSystem(new GravitySystem(-9.8f));

	Entity* ent = world->create();
	ent->assign<Position>(0.f, 0.f); // assign() takes arguments and passes them to the constructor
	ent->assign<Rotation>(35.f);

	float dt = 30.0f / 60.0f;

	unsigned int cc = 0;

	while (cc++ < 5)
	{
		world->tick(dt);
	}

	world->destroyWorld();
}