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

void GameStatesController::DoOnUpdate(float deltaTime)
{
	StateUpdate([deltaTime](GameStateBase* state) { state->OnForegroundUpdate(deltaTime); }, [deltaTime](GameStateBase* state) { state->OnBackgroundUpdate(deltaTime); });
}

void GameStatesController::DoOnLateUpdate(float deltaTime)
{
	StateUpdate([deltaTime](GameStateBase* state) { state->OnForegroundLateUpdate(deltaTime); }, [deltaTime](GameStateBase* state) { state->OnBackgroundLateUpdate(deltaTime); });
}

void GameStatesController::DoOnFixedUpdate(float fixedDeltaTime)
{
	StateUpdate([fixedDeltaTime](GameStateBase* state) { state->OnForegroundFixedUpdate(fixedDeltaTime); }, [fixedDeltaTime](GameStateBase* state) { state->OnBackgroundFixedUpdate(fixedDeltaTime); });
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

void GameStatesController::PushStateImpl(GameStateBase* newState)
{
	if (!m_states.empty())
	{
		GameStateBase* oldHead = m_states.back();
		oldHead->OnExitForeground();
		oldHead->OnEnterBackground();
	}

	m_states.push_back(newState);

	newState->OnEnter();
	newState->OnEnterForeground();
}


GameStatesController::~GameStatesController()
{
	PopAll();
}
