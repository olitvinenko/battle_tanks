#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <utility>

#include "GameStateBase.h"

class GameStatesController final
{
public:
	void DoOnUpdate(float deltaTime);

	void DoOnLateUpdate(float deltaTime);

	void DoOnFixedUpdate(float fixedDeltaTime);

	template<typename State, typename ... Args>
	void PushState(Args ... args);

	template<typename State>
	void PushState();

	template<typename State, typename ... Args>
	void SwitchState(Args ... args);

	void PopState();

	GameStateBase* PeekState() const;

	void PopAll();

	~GameStatesController();
private:
	std::vector<GameStateBase*> m_states;

	void PushStateImpl(GameStateBase* newState);
	void StateUpdate(const std::function<void(GameStateBase*)>& foregroundUpdate, const std::function<void(GameStateBase*)>& backgroundUpdate);
};

template<typename State, typename ... Args>
void GameStatesController::PushState(Args ... args)
{
	GameStateBase* newState = new State(this, std::forward<Args...>(args...));
	PushStateImpl(newState);
}

template<typename State>
void GameStatesController::PushState()
{
	GameStateBase* newState = new State(this);
	PushStateImpl(newState);
}

template<typename State, typename ... Args>
void GameStatesController::SwitchState(Args ... args)
{
	PopState();
	PushState<State>(args...);
}