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

	//Raven_Game& GetGame() override { return *m_game; };
	void FixedUpdate(float dt) override;

private:
	//std::unique_ptr<Raven_Game> m_game;
};
