#pragma once

#include "ThreadPool.h"
#include "GameStatesController.h"
#include "GameLoop.h"

struct IClipboard;
struct IWindow;
struct IInput;

namespace FileSystem
{
	class IFileSystem;
}


//TODO:: 
class RenderingEngine final : public IRenderable
{
public:
	void Render(float interpolation) const override
	{
		//TODO:: 
	}
};

//TODO:: 
class PhysicsEngine final : public IFixedUpdatable
{
public:
	void FixedUpdate(float fixedDeltaTime) override
	{
		//TODO:: 
	}
};


class Engine final
	: public IFixedUpdatable
	, public IUpdatable
	, public IRenderable
{
public:
	explicit Engine(IWindow* window, IClipboard* clipboard, IInput* input);

	void Update(float realDeltaTime) override;
	void FixedUpdate(float fixedDeltaTime) override;
	void Render(float interpolation) const override;

	void Launch();
	void Shutdown();

	~Engine();

private:
	IWindow* m_window;
	IClipboard* m_clipboard;
	IInput* m_input;

	GameLoop m_loop;

	ThreadPool m_threadPool;
	GameStatesController m_statesController;

	bool m_running;
};
