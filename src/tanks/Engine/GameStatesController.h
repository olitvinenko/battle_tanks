#pragma once

#include <functional>
#include <vector>
#include <memory>

#include "GameStateBase.h"


class GameStatesController final : public std::enable_shared_from_this<GameStatesController>
{
public:
	void DoOnUpdate();

	void DoOnLateUpdate();

	void DoOnFixedUpdate();

	template<typename State, typename ... Args>
	void PushState(Args ... args);

	template<typename State, typename ... Args>
	void SwitchState(Args ... args);

	void PopState();

	GameStateBase* PeekState() const;

	void PopAll();

	~GameStatesController();
private:
	std::vector<GameStateBase*> m_states;

	void StateUpdate(const std::function<void(GameStateBase*)>& foregroundUpdate, const std::function<void(GameStateBase*)>& backgroundUpdate);
};

template<typename State, typename ... Args>
void GameStatesController::PushState(Args ... args)
{
	if (m_states.size() > 0)
	{
		GameStateBase* oldHead = m_states.back();
		oldHead->OnExitForeground();
		oldHead->OnEnterBackground();
	}

	GameStateBase* newState = new State(shared_from_this(), std::forward<Args...>(args...));
	m_states.push_back(newState);

	newState->OnEnter();
	newState->OnEnterForeground();
}


template<typename State, typename ... Args>
void GameStatesController::SwitchState(Args ... args)
{
	PopState();
	PushState<State>(args...);
}