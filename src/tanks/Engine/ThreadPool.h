#pragma once 

#include <thread> 
#include <future> 
#include <vector> 
#include <queue> 

class ThreadPool final
{
public:
	ThreadPool();
	explicit ThreadPool(size_t threadsCount);
	~ThreadPool();

	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>;

private:
	// to join() call 
	std::vector<std::thread> m_workers;

	// the task queue 
	std::queue<std::function<void()>> m_tasks;

	// synchronization 
	std::mutex m_queue_mutex;
	std::condition_variable m_condition;
	bool m_stop;
};

template<class F, class... Args> 
auto ThreadPool::enqueue(F&& f, Args&&... args)->std::future<typename std::result_of<F(Args...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared < std::packaged_task < return_type()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
	);

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(m_queue_mutex);

		// don't allow enqueueing after stopping the pool 
		if (m_stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");

		m_tasks.emplace([task]() { (*task)(); });
	}
	m_condition.notify_one();
	return res;
}