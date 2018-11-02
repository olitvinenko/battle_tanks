#include "Engine.h"
#include "GameController.h"
#include "Config.h"
#include "LoadDataState.h"

#include <memory>

GameController::GameController(Engine& engine)
	: m_engine(engine)
{
}

void GameController::Launch()
{
	// init file system
	// init texture manager
	TextureManager& tm = m_engine.GetRender().GetTextureManager();
	std::shared_ptr<FileSystem::IFileSystem> fs = m_engine.GetFileSystem();

	if (tm.LoadPackage(FILE_TEXTURES, fs->Open(FILE_TEXTURES)->AsMemory(), *fs) <= 0)
	{
		//logger.Printf(1, "WARNING: no textures loaded");
	}

	if (tm.LoadDirectory(DIR_SKINS, "skin/", *fs) <= 0)
	{
		//logger.Printf(1, "WARNING: no skins found");
	}

	m_engine.GetStatesController().PushState<LoadDataState>();
}

void GameController::Shutdown()
{

}