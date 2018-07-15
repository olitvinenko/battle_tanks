#pragma once

#include <memory>
#include "GameContextBase.h"

class TextureManager;

namespace FileSystem
{
	class File;
}

class EditorContext : public GameContextBase
{
public:
	EditorContext(std::shared_ptr<FileSystem::File> stream, TextureManager& texManager);
	EditorContext(int width, int height, TextureManager& texManager);

	~EditorContext() = default;

	Pathfinder& GetPathfinder() override { return *m_pathfinder; };
	void FixedUpdate(float dt) override;

private:
	std::unique_ptr<Pathfinder> m_pathfinder;
};
