#pragma once

#include <chrono>
#include <thread>

class task
{
public:
	task() = default;
	task(unsigned int time_to_complete);

	bool is_tired() const;

	void perform() const;

public:
	bool operator< (const task& rhs) const
	{
		return this->time_to_complete < rhs.time_to_complete;
	}

	bool operator> (const task& rhs) const
	{
		return this->time_to_complete > rhs.time_to_complete;
	}

	bool operator== (const task& rhs) const
	{
		return this->creation_time == rhs.creation_time;
	}

private:
	unsigned int time_to_complete;
	std::chrono::steady_clock::time_point creation_time;
};