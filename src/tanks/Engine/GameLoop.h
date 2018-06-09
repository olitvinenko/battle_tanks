#pragma once

#include <unordered_set>
#include <cassert>
#include <chrono>
#include <algorithm>

struct IUpdatable
{
	virtual void Update(float realDeltaTime) = 0;
	virtual ~IUpdatable() = default;
};

struct IFixedUpdatable
{
	virtual void FixedUpdate(float fixedDeltaTime) = 0;
	virtual ~IFixedUpdatable() = default;
};

struct IRenderable
{
	virtual void Render(float interpolation) = 0;
	virtual ~IRenderable() = default;
};


class Time
{
public:
	static size_t GetTickCount()
	{
		auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
	}
};

class GameLoop
{
	template<typename T>
	class GameLoopSet
	{
	public:
		void Register(T* item)
		{
			std::pair <typename std::unordered_set<T*>::iterator, bool> pair = m_targets.insert(item);
			assert(pair.second);
		}

		void Unregister(T* item)
		{
			assert(m_targets.erase(item));
		}

		template<typename F>
		void Foreach(F& f)
		{
			std::for_each(m_targets.begin(), m_targets.end(), f);
		}

		~GameLoopSet()
		{
			assert(m_targets.size() == 0);
		}

	private:
		//TODO: order?
		std::unordered_set<T*> m_targets;
	};

	const int MS_IN_SEC = 1000;
	const int FPS = 30;
	const float MS_PER_UPDATE = MS_IN_SEC / FPS;
	const int SKIP_FRAMES_MAX = 5;

public:
	GameLoop()
		: m_lastTime(0.0)
		, m_lag(0.0)
	{ }

	void Start()
	{
		m_lastTime = Time::GetTickCount();
		m_lag = 0.0;
	}

	void Tick()
	{
		float current = Time::GetTickCount();
		float elapsed = current - m_lastTime;

		m_lastTime = current;
		m_lag += elapsed;

		float realElapsedSec = elapsed / MS_IN_SEC;
		m_updatables.Foreach([realElapsedSec](IUpdatable* f) { f->Update(realElapsedSec); });

		int loops = 0;
		while (m_lag >= MS_PER_UPDATE && loops < SKIP_FRAMES_MAX)
		{
			float fixedDeltaTime = MS_PER_UPDATE / MS_IN_SEC;
			m_fixedUpdatables.Foreach([fixedDeltaTime](IFixedUpdatable* f) { f->FixedUpdate(fixedDeltaTime); });

			m_lag -= MS_PER_UPDATE;
			loops++;
		}

		float interpolation = m_lag / MS_PER_UPDATE;
		m_renderables.Foreach([interpolation](IRenderable* f) { f->Render(interpolation); });
	}

	template<typename T>
	void Add(T*) { assert(false); }

	template<typename T>
	void Remove(T*) { assert(false); }
private:
	float m_lastTime;
	float m_lag;

	GameLoopSet<IUpdatable> m_updatables;
	GameLoopSet<IRenderable> m_renderables;
	GameLoopSet<IFixedUpdatable> m_fixedUpdatables;
};

template<>
inline void GameLoop::Add<IUpdatable>(IUpdatable* item)
{
	m_updatables.Register(item);
}

template<>
inline void GameLoop::Remove<IUpdatable>(IUpdatable* item)
{
	m_updatables.Unregister(item);
}

template<>
inline void GameLoop::Add<IRenderable>(IRenderable* item)
{
	m_renderables.Register(item);
}

template<>
inline void GameLoop::Remove<IRenderable>(IRenderable* item)
{
	m_renderables.Unregister(item);
}

template<>
inline void GameLoop::Add<IFixedUpdatable>(IFixedUpdatable* item)
{
	m_fixedUpdatables.Register(item);
}

template<>
inline void GameLoop::Remove<IFixedUpdatable>(IFixedUpdatable* item)
{
	m_fixedUpdatables.Unregister(item);
}