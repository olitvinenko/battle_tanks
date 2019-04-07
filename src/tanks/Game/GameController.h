#pragma once

#include "LayoutManager.h"
#include "Singleton.h"
#include "GameLoop.h"
#include  "Engine.h"

class ThreadPool;

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

	std::shared_ptr<ThreadPool> GetThreadPool() const { return m_engine.GetThreadPool(); }
	std::shared_ptr<FileSystem::IFileSystem> GetFileSystem() const { return m_engine.GetFileSystem(); }
	std::shared_ptr<RenderingEngine> GetRenderingEngine() const { return m_engine.GetRenderingEngine(); }
private:

	Engine& m_engine;
	LayoutManager m_layoutManager;
};