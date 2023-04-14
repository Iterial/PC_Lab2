#include "task_queue.h"

void task_queue::clear()
{
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	while (!tasks.empty())
	{
		tasks.pop();
	}
}

bool task_queue::empty() const
{
	std::shared_lock<std::shared_mutex> lock(rw_lock);
	return tasks.empty();
}

size_t task_queue::size() const
{
	std::shared_lock<std::shared_mutex> lock(rw_lock);
	return tasks.size();
}

void task_queue::push(const task& value)
{
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	tasks.emplace(value);
}

bool task_queue::pop(task& value)
{
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	if (tasks.empty())
	{
		return false;
	}
	else
	{
		value = tasks.top();
		tasks.pop();
		return true;
	}
}

bool task_queue::remove_tired(std::vector<task>& tired_tasks)
{
	std::unique_lock<std::shared_mutex> lock(rw_lock);
	try
	{
		tasks.remove_tired(tired_tasks);
		return true;
	}
	catch (std::logic_error& e)
	{
		std::cout << "Error in task queue while removing tired tasks: " << e.what() << std::endl;
		return false;
	}
}