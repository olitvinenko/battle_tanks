#include "GameStateBase.h"

GameStateBase::GameStateBase(GameStatesController* controller)
	: m_controller(controller)
{
}

void GameStateBase::OnUpdate(float deltaTime) { }
void GameStateBase::OnLateUpdate(float deltaTime) { }
void GameStateBase::OnFixedUpdate(float fixedDeltaTime) { }

void GameStateBase::OnEnter() { }
void GameStateBase::OnEnterForeground() { }
void GameStateBase::OnEnterBackground() { }

void GameStateBase::OnExit() { }
void GameStateBase::OnExitForeground() { }
void GameStateBase::OnExitBackground() { }

void GameStateBase::OnForegroundUpdate(float deltaTime) { OnUpdate(deltaTime); }
void GameStateBase::OnBackgroundUpdate(float deltaTime) { OnUpdate(deltaTime); }

void GameStateBase::OnForegroundLateUpdate(float deltaTime) { OnLateUpdate(deltaTime); }
void GameStateBase::OnBackgroundLateUpdate(float deltaTime) { OnLateUpdate(deltaTime); }

void GameStateBase::OnForegroundFixedUpdate(float fixedDeltaTime) { OnFixedUpdate(fixedDeltaTime); }
void GameStateBase::OnBackgroundFixedUpdate(float fixedDeltaTime) { OnFixedUpdate(fixedDeltaTime); }

GameStateBase::~GameStateBase() = default;
