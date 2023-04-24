#pragma once

#include <vector>
#include <chrono>
#include <thread>
#include <shared_mutex>

#include "task_queue.h"
#include "task.h"

#define WORKERS1_COUNT 3
#define WORKERS2_COUNT 1

class thread_pool
{
public:
	thread_pool() = default;
	~thread_pool() { terminate(); }

	bool initialise(const size_t workers1_count = WORKERS1_COUNT,
		const size_t workers2_count = WORKERS2_COUNT);
	void terminate(bool hard_termination = false);

public:
	bool pause();
	bool resume();

public:
	bool add_task(const task& t);

public:
	bool working() const;
	bool is_paused() const;

public:
	size_t get_first_queue_size() const;
	size_t get_second_queue_size() const;

public:
	thread_pool(thread_pool& other) = delete;
	thread_pool(thread_pool&& other) = delete;
	thread_pool& operator=(thread_pool& rhs) = delete;
	thread_pool& operator=(thread_pool&& rhs) = delete;

private:
	void routine1();
	void routine2();

private:
	void move_tired();

private:
	bool working_unsafe() const;
	bool is_paused_unsafe() const;

private:
	void reset_unsafe();

private:
	mutable std::shared_mutex rw_lock;
	mutable std::condition_variable_any task_waiter1;
	mutable std::condition_variable_any task_waiter2;

	std::vector<std::thread> workers1;
	std::vector<std::thread> workers2;

	task_queue tasks1;
	task_queue tasks2;

	bool initialized = false;
	bool paused = false;
	bool terminated = false;
};