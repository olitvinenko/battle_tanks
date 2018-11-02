#pragma once

class Engine;

class GameController final
{
public:
	explicit GameController(Engine& engine);

	void Launch();

	void Shutdown();
private:
	Engine& m_engine;
};