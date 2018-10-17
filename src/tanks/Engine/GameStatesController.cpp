#include "GameStatesController.h"


void GameStatesController::PopState()
{
	if (m_states.empty())
		return;

	GameStateBase* oldHead = m_states.back();
	m_states.pop_back();

	oldHead->OnExitForeground();
	oldHead->OnExit();

	delete oldHead;

	if (m_states.empty())
		return;

	GameStateBase* newHead = m_states.back();
	newHead->OnExitBackground();
	newHead->OnEnterForeground();
}

GameStateBase* GameStatesController::PeekState() const
{
	return m_states.back();
}

void GameStatesController::PopAll()
{
	while (!m_states.empty())
		PopState();
}

void GameStatesController::DoOnUpdate()
{
	StateUpdate([](GameStateBase* state) { state->OnForegroundUpdate(); }, [](GameStateBase* state) { state->OnBackgroundUpdate(); });
}

void GameStatesController::DoOnLateUpdate()
{
	StateUpdate([](GameStateBase* state) { state->OnForegroundLateUpdate(); }, [](GameStateBase* state) { state->OnBackgroundLateUpdate(); });
}

void GameStatesController::DoOnFixedUpdate()
{
	StateUpdate([](GameStateBase* state) { state->OnForegroundFixedUpdate(); }, [](GameStateBase* state) { state->OnBackgroundFixedUpdate(); });
}

void GameStatesController::StateUpdate(const std::function<void(GameStateBase*)>& foregroundUpdate, const std::function<void(GameStateBase*)>& backgroundUpdate)
{
	if (m_states.empty())
		return;

	foregroundUpdate(m_states.back());

	if (m_states.empty())
		return;

	for (int i = m_states.size() - 2; i >= 0; i--)
	{
		if (i + 1 > m_states.size()) //previous state could have poped other states, so we can not update them anymore
		{
			i = m_states.size();
			continue;
		}

		backgroundUpdate(m_states[i]);
	}
}

GameStatesController::~GameStatesController()
{
	PopAll();
}
