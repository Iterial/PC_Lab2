#pragma once

#include <vector>
#include <shared_mutex>
#include <iostream>
#include <stdexcept>

#include "custom_priority_queue.h"
#include "task.h"

class task_queue
{
public:
	task_queue() = default;
	~task_queue() { clear(); }

	void clear();

public:
	bool empty() const;
	size_t size() const;

public:
	void push(const task& value);
	bool pop(task& value);
	bool remove_tired(std::vector<task>& tired_tasks);

public:
	task_queue(task_queue& other) = delete;
	task_queue(task_queue&& other) = delete;
	task_queue& operator=(task_queue& rhs) = delete;
	task_queue& operator=(task_queue&& rhs) = delete;

private:
	mutable std::shared_mutex rw_lock;
	custom_priority_queue tasks;
};