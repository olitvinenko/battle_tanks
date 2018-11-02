#include "MainMenuState.h"
#include <ostream>
#include <iostream>

MainMenuState::MainMenuState(GameStatesController* controller)
	: GameStateBase(controller)
{
}

void MainMenuState::OnEnter()
{
	std::cout << "MainMenuState::OnEnter()" << std::endl;
}
