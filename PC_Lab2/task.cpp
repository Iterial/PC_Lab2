#include "task.h"

task::task(int id, unsigned int time_to_complete)
{
	this->id = id;
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

	auto completion_time = std::chrono::high_resolution_clock::now();
	double elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(completion_time - creation_time).count() * 1e-9;
	printf("Task #%d is completed in %.9f seconds\n", id, elapsed);
}