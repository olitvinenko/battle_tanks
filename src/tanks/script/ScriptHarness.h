#pragma once
#include "sPickup.h"
#include "sPlayer.h"
#include "sRigidBodyStatic.h"
#include "sTrigger.h"
#include "gc/WorldEvents.h"
#include <memory>

class SaveFile;
class World;
struct ScriptMessageSink;
struct lua_State;

namespace FileSystem
{
	class Stream;
}

class ScriptHarness
	: ObjectListener<World>
{
public:
	explicit ScriptHarness(World &world, ScriptMessageSink &messageSink);
	~ScriptHarness();

	void Step(float dt);

	void Serialize(SaveFile &f);
	void Deserialize(SaveFile &f);

private:
	World &_world;
    ScriptMessageSink &_messageSink;
	lua_State *_L;

	sPickup _sPickup;
	sPlayer _sPlayer;
	sRigidBodyStatic _sRigidBodyStatic;
	sTrigger _sTrigger;

	// ObjectListener<World>
	void OnGameStarted() override;
	void OnGameFinished() override {}
	void OnKill(GC_Object &) override {}
	void OnNewObject(GC_Object &) override {}
};
