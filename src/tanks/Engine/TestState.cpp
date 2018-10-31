#include "TestState.h"

TestState::TestState(std::shared_ptr<GameStatesController> controller)
	: GameStateBase(controller)
{
}

void TestState::OnUpdate(float deltaTime)
{
}

void TestState::OnLateUpdate(float deltaTime)
{
}

void TestState::OnFixedUpdate(float fixedDeltaTime)
{
}

void TestState::OnEnter()
{
}

void TestState::OnExit()
{
}
