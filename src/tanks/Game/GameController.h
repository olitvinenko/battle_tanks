#pragma once

#include "LayoutManager.h"
#include "Singleton.h"
#include "GameLoop.h"

class Engine;


class GameController final
	: public Singleton<GameController>
	, public IUpdatable
{
public:
	explicit GameController(Engine& engine);

	void Launch();
	void Shutdown();

	void Update(float realDeltaTime) override;

	LayoutManager& GetLayoutManager() { return m_layoutManager; }
private:

	Engine& m_engine;
	LayoutManager m_layoutManager;
};