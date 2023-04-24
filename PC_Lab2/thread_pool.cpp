#include "thread_pool.h"

bool thread_pool::initialise(const size_t workers1_count, const size_t workers2_count)
{
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	if (initialized || terminated)
	{
		return false;
	}

	workers1.reserve(workers1_count);
	for (size_t id = 0; id < workers1_count; id++)
	{
		workers1.emplace_back(&thread_pool::routine1, this);
	}

	workers2.reserve(workers2_count);
	for (size_t id = 0; id < workers2_count; id++)
	{
		workers2.emplace_back(&thread_pool::routine2, this);
	}

	initialized = !workers1.empty() && !workers2.empty();
	return initialized;
}

void thread_pool::terminate(bool hard_termination)
{
	{
		std::unique_lock<std::shared_mutex> lock(rw_lock);
		if (working_unsafe())
		{
			terminated = true;
		}
		else
		{
			reset_unsafe();
			return;
		}
	}

	if (hard_termination)
	{
		tasks1.clear();
		tasks2.clear();
	}

	task_waiter1.notify_all();
	task_waiter2.notify_all();

	for (auto& worker : workers1)
	{
		worker.join();
	}
	for (auto& worker : workers2)
	{
		worker.join();
	}

	reset_unsafe();
}

bool thread_pool::pause()
{
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	if (!initialized || paused || terminated)
	{
		return false;
	}
	else
	{
		paused = true;
		return true;
	}
}

bool thread_pool::resume()
{
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	if (!initialized || !paused || terminated)
	{
		return false;
	}
	else
	{
		paused = false;
		task_waiter1.notify_all();
		task_waiter2.notify_all();
		return true;
	}
}

bool thread_pool::add_task(const task& t)
{
	if (!working())
	{
		return false;
	}
	else
	{
		tasks1.push(t);

		{
			std::shared_lock<std::shared_mutex> lock(rw_lock);
			if (!paused)
			{
				task_waiter1.notify_one();
			}
		}

		return true;
	}
}

bool thread_pool::working() const
{
	std::shared_lock<std::shared_mutex> lock(rw_lock);
	return working_unsafe();
}

bool thread_pool::is_paused() const
{
	std::shared_lock<std::shared_mutex> lock(rw_lock);
	return is_paused_unsafe();
}

size_t thread_pool::get_first_queue_size() const
{
	return tasks1.size();
}

size_t thread_pool::get_second_queue_size() const
{
	return tasks2.size();
}

void thread_pool::routine1()
{
	while (true)
	{
		bool task_acquiered = false;
		task t;

		{
			std::unique_lock<std::shared_mutex> lock(rw_lock);
			auto wait_condition = [this, &task_acquiered, &t] {
				if (!task_acquiered)
				{
					task_acquiered = tasks1.pop(t);
				}
				return terminated || (!paused && task_acquiered);
			};

			auto begin = std::chrono::high_resolution_clock::now();

			task_waiter1.wait(lock, wait_condition);

			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() * 1e-9;

			printf("Thread %u (from 1st queue) was waiting for %.9f seconds.\n", std::this_thread::get_id(), elapsed);
		}

		if (task_acquiered)
		{
			move_tired();
			t.perform();
		}
		else
		{
			std::shared_lock<std::shared_mutex> lock(rw_lock);
			if (terminated)
			{
				return;
			}
		}
	}
}

void thread_pool::routine2()
{
	while (true)
	{
		bool task_acquiered = false;
		task t;

		{
			std::unique_lock<std::shared_mutex> lock(rw_lock);
			auto wait_condition = [this, &task_acquiered, &t] {
				if (!task_acquiered)
				{
					task_acquiered = tasks2.pop(t);
				}
				return terminated || (!paused && task_acquiered);
			};

			auto begin = std::chrono::high_resolution_clock::now();

			task_waiter2.wait(lock, wait_condition);

			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() * 1e-9;

			printf("Thread %u (from 2d queue) was waiting for %.9f seconds.\n", std::this_thread::get_id(), elapsed);
		}

		if (task_acquiered)
		{
			t.perform();
		}
		else
		{
			std::shared_lock<std::shared_mutex> lock(rw_lock);
			if (terminated)
			{
				return;
			}
		}
	}
}

void thread_pool::move_tired()
{
	std::shared_lock<std::shared_mutex> lock(rw_lock);
	if (terminated)
	{
		return;
	}

	std::vector<task> tired_tasks;
	tasks1.remove_tired(tired_tasks);

	for (const auto& t : tired_tasks)
	{
		tasks2.push(t);

		if (!paused)
		{
			task_waiter2.notify_one();
		}
	}
}

bool thread_pool::working_unsafe() const
{
	return initialized && !terminated;
}

bool thread_pool::is_paused_unsafe() const
{
	return initialized && paused && !terminated;
}

void thread_pool::reset_unsafe()
{
	workers1.clear();
	workers2.clear();

	tasks1.clear();
	tasks2.clear();

	initialized = false;
	paused = false;
	terminated = false;
}