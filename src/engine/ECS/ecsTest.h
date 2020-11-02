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
	, public EventListener<MyEvent>
	, public EventListener<OnComponentAssigned<Position>>
	, public EventListener<OnComponentRemoved<Position>>
	, public EventListener<OnEntityCreated>
	, public EventListener<OnEntityDestroyed>
{
public:
	explicit GravitySystem(float amount)
	{
		gravityAmount = amount;
	}

	void Configure(ECSWorld* world) override
	{
		world->Subscribe<OnComponentAssigned<Position>>(this);
		world->Subscribe<OnComponentRemoved<Position>>(this);
		world->Subscribe<OnEntityCreated>(this);
		world->Subscribe<OnEntityDestroyed>(this);
		world->Subscribe<MyEvent>(this);
	}

	void Unconfigure(ECSWorld* world) override
	{
		world->Unsubscribe<OnComponentAssigned<Position>>(this);
		world->Unsubscribe<OnComponentRemoved<Position>>(this);
		world->Unsubscribe<OnEntityCreated>(this);
		world->Unsubscribe<OnEntityDestroyed>(this);
		world->Unsubscribe<MyEvent>(this);
	}

	void Receive(ECSWorld* world, const MyEvent& event) override
	{
		std::cout << "receive::MyEvent" << std::endl;
	}

	void Receive(ECSWorld* world, const OnComponentAssigned<Position>& event) override
	{
		std::cout << "receive::OnComponentAssigned" << std::endl;
	}

	void Receive(ECSWorld* world, const OnComponentRemoved<Position>& event) override
	{
		std::cout << "receive::OnComponentRemoved" << std::endl;
	}

	void Receive(ECSWorld* world, const OnEntityCreated& event) override
	{
		std::cout << "receive::OnEntityCreated" << std::endl;
	}

	void Receive(ECSWorld* world, const OnEntityDestroyed& event) override
	{
		std::cout << "receive::OnEntityDestroyed" << std::endl;
	}

	void Tick(ECSWorld* world, float deltaTime) override
	{
		world->Each<Position>([&](Entity* ent, Component<Position> position) {
			position->y += gravityAmount * deltaTime;
		});

		world->Emit<MyEvent>({ 123, 45.67f }); // you can use initializer syntax if you want, this sets foo = 123 and bar = 45.67f
	}

private:
	float gravityAmount;
};

//TODO:: a.litvinenko: for testing only
void ECSTest()
{
	ECSWorld* world = ECSWorld::CreateWorld();
	world->RegisterSystem(new GravitySystem(-9.8f));

	Entity* ent = world->Create();
	ent->Assign<Position>(0.f, 0.f); // assign() takes arguments and passes them to the constructor
	ent->Assign<Rotation>(35.f);

	float dt = 30.0f / 60.0f;

	unsigned int cc = 0;

	while (cc++ < 5)
	{
		world->Tick(dt);
	}

	world->DestroyWorld();
}
