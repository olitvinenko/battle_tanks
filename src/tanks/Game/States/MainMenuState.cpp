#include "MainMenuState.h"
#include "GameController.h"
#include <ostream>
#include <iostream>

MainMenuState::MainMenuState(GameStatesController* controller)
	: GameStateBase(controller)
{
}

void MainMenuState::OnEnter()
{
	GameController* inst = GameController::GetInstance();


	std::cout << "MainMenuState::OnEnter()" << std::endl;
}
