#pragma once

#include "GameContext.h"
#include <MyMath.h>
#include <memory>

namespace FileSystem
{
    class Stream;
}

class World;

class EditorContext : public GameContextBase
{
public:
	EditorContext(int width, int height, FileSystem::Stream *stream = nullptr);
	virtual ~EditorContext();

	FRECT GetOriginalBounds() const { return _originalBounds; }

	// GameContextBase
	World& GetWorld() override { return *_world; }
	Gameplay* GetGameplay() override { return nullptr; }
	void Step(float dt) override;

private:
	std::unique_ptr<World> _world;
	FRECT _originalBounds = {};
};
