#include "task.h"

task::task(unsigned int time_to_complete)
{
	this->time_to_complete = time_to_complete;
	creation_time = std::chrono::high_resolution_clock::now();
}

bool task::is_tired() const
{
	auto current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = current_time - creation_time;

	return time_to_complete * 2 <= elapsed.count();
}

void task::perform() const
{
	std::this_thread::sleep_for(std::chrono::seconds(time_to_complete));
}