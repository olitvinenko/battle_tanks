#include "ThreadPool.h" 

ThreadPool::ThreadPool()
	: ThreadPool(std::thread::hardware_concurrency())
{
}

ThreadPool::ThreadPool(size_t threadsCount)
	: m_stop(false)
{
	for (size_t i = 0; i < threadsCount; ++i)
	{
		m_workers.emplace_back([this]
		{
			while (true)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(m_queue_mutex);
					m_condition.wait(lock, [this] { return m_stop || !m_tasks.empty(); });

					if (m_stop && m_tasks.empty())
						return;

					task = std::move(m_tasks.front());
					m_tasks.pop();
				}

				task();
			}
		}
		);
	}
}

ThreadPool::~ThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(m_queue_mutex);
		m_stop = true;
	}

	m_condition.notify_all();
	for (std::thread &worker : m_workers)
		worker.join();
}
