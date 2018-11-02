#pragma once

#include "LayoutManager.h"
#include "Singleton.h"

class Engine;


class GameController final : public Singleton<GameController>
{
public:
	explicit GameController(Engine& engine);

	void Launch();
	void Shutdown();

	LayoutManager& GetLayoutManager() { return m_layoutManager; }
private:

	Engine& m_engine;
	LayoutManager m_layoutManager;
};