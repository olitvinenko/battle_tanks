#include "LoadDataState.h"
#include "MainMenuState.h"
#include "GameController.h"
#include "Config.h"


LoadDataState::LoadDataState(GameStatesController* controller)
	: GameStateBase(controller)
	, m_texturesLoaded(false)
{
}

void LoadDataState::OnEnter()
{
	//TODO::
	//1. load indicator image
	//2. start its spinning while all packages are loading
	//3. wait in update method

	GameController* inst = GameController::GetInstance();
	//TODO:: make possible to call render::textCreate in the separate thread
	//inst->GetThreadPool()->enqueue(&LoadDataState::InitTextureManager, this);

	InitTextureManager();
}

void LoadDataState::InitTextureManager()
{
	GameController* inst = GameController::GetInstance();

	TextureManager& tm = inst->GetRenderingEngine()->GetTextureManager();
	std::shared_ptr<FileSystem::IFileSystem> fs = inst->GetFileSystem();

	if (tm.LoadPackage(FILE_TEXTURES, fs->Open(FILE_TEXTURES)->AsMemory(), *fs) <= 0)
	{
		//logger.Printf(1, "WARNING: no textures loaded");
	}

	if (tm.LoadDirectory(DIR_SKINS, "skin/", *fs) <= 0)
	{
		//logger.Printf(1, "WARNING: no skins found");
	}

	m_texturesLoaded = true;
}

void LoadDataState::OnUpdate(float deltaTime)
{
	if (!m_texturesLoaded)
	{
		//TODO:: logger
		std::cout << "!m_texturesLoaded" << std::endl;
		return;
	}

	GameController::GetInstance()->GetLayoutManager().Initialize();

	// switching to MainMenuState can be from any other state
	m_controller->SwitchState<MainMenuState>();
}
