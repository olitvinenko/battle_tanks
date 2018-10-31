#include "LoadDataState.h"
#include "GameStatesController.h"
#include "MainMenuState.h"

LoadDataState::LoadDataState(GameStatesController* controller)
	: GameStateBase(controller)
	, m_timeElapsed(0.0f)
{
}

void LoadDataState::OnEnter()
{
	//TODO:

	// show loading indicator
	// start FS initializing in the separate thread
	// wait it in the update method
}

void LoadDataState::OnUpdate(float deltaTime)
{
	m_timeElapsed += deltaTime;
	if (m_timeElapsed >= 5)
		m_controller->SwitchState<MainMenuState>();
}
