#pragma once

#include <memory>
#include "GameContextBase.h"

class World;

namespace FileSystem
{
	struct IStream;
}

class EditorContext : public GameContextBase
{
public:
	explicit EditorContext(FileSystem::IStream &stream);
	EditorContext(int width, int height);

	~EditorContext() = default;

	World& GetWorld() override { return *_world; }
	void FixedUpdate(float dt) override;

private:
	std::unique_ptr<World> _world;
};
