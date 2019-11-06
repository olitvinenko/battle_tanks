#pragma once

#include <memory>
#include "GameContextBase.h"
#include "Rect.h"

class TextureManager;

namespace FileSystem
{
	class File;
}

class World;

class EditorContext : public GameContextBase
{
public:
	EditorContext(std::shared_ptr<FileSystem::File> stream, TextureManager& texManager);
	EditorContext(int width, int height, TextureManager& texManager);

	~EditorContext() = default;

	std::shared_ptr<Pathfinder> GetPathfinder() override { return m_pathfinder; };
    World& GetWorld() override { return *_world; }
    
    RectFloat GetOriginalBounds() const { return _originalBounds; }
    
	void FixedUpdate(float dt) override;

private:
	std::shared_ptr<Pathfinder> m_pathfinder;
    RectFloat _originalBounds = {};
    std::unique_ptr<World> _world;
};
