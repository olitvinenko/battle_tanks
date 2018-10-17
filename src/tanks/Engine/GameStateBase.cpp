#include "GameStateBase.h"

GameStateBase::GameStateBase(std::shared_ptr<GameStatesController> controller)
	: m_controller(controller)
{
}

void GameStateBase::OnUpdate() { }
void GameStateBase::OnLateUpdate() { }
void GameStateBase::OnFixedUpdate() { }

void GameStateBase::OnEnter() { }
void GameStateBase::OnEnterForeground() { }
void GameStateBase::OnEnterBackground() { }

void GameStateBase::OnExit() { }
void GameStateBase::OnExitForeground() { }
void GameStateBase::OnExitBackground() { }

void GameStateBase::OnForegroundUpdate() { OnUpdate(); }
void GameStateBase::OnBackgroundUpdate() { OnUpdate(); }

void GameStateBase::OnForegroundLateUpdate() { OnLateUpdate(); }
void GameStateBase::OnBackgroundLateUpdate() { OnLateUpdate(); }

void GameStateBase::OnForegroundFixedUpdate() { OnFixedUpdate(); }
void GameStateBase::OnBackgroundFixedUpdate() { OnFixedUpdate(); }

GameStateBase::~GameStateBase() = default;
