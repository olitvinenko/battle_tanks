#include "Engine.h"
#include "GameController.h"
#include "States/LoadDataState.h"

GameController::GameController(Engine& engine)
	: m_engine(engine)
	, m_layoutManager(engine.GetRenderingEngine(), engine.GetFileSystem(), engine.GetInput(), engine.GetClipboard())
{
}

void GameController::Launch()
{
	m_engine.GetLoop().Add<IUpdatable>(this);

	m_engine.GetStatesController().PushState<LoadDataState>();
}

void GameController::Update(float realDeltaTime)
{
	m_layoutManager.Update(realDeltaTime);

	//TODO:: update model
}

void GameController::Shutdown()
{
	m_engine.GetLoop().Remove<IUpdatable>(this);
}
