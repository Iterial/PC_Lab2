#include <random>
#include <thread>
#include <chrono>
#include <iostream>

#include "thread_pool.h"

#define TASKS_AMOUNT 30
#define TASK_MIN_TIME 1
#define TASK_MAX_TIME 1
#define TASK_ADDING_DELAY_MIN 0
#define TASK_ADDING_DELAY_MAX 0
#define TERMINATION_DELAY 20
#define TERMINATION_MODE true

#define RESUME_DELAY_MIN 5
#define RESUME_DELAY_MAX 5
#define PAUSE_DELAY_MIN 0
#define PAUSE_DELAY_MAX 7

#define QUEUE_SIZE_READ_DELAY 8

#define THREADS_START_DELAY 2

std::random_device rd;
std::mt19937 mersenne(rd());

void add_tasks_routine(thread_pool& tp)
{
	auto dist_task = std::uniform_int_distribution<int>(TASK_MIN_TIME, TASK_MAX_TIME);
	auto dist_wait = std::uniform_int_distribution<int>(TASK_ADDING_DELAY_MIN, TASK_ADDING_DELAY_MAX);

	for (int i = 0; i < TASKS_AMOUNT; i++)
	{
		task t = task(i, dist_task(mersenne));
		if (tp.add_task(t))
		{
			printf("Task #%d is added\n", i);
		}
		else
		{
			printf("Failed to add task #%d\n", i);
		}

		std::this_thread::sleep_for(std::chrono::seconds(dist_wait(mersenne)));
	}

	std::this_thread::sleep_for(std::chrono::seconds(TERMINATION_DELAY));

	std::cout << "Terminating thread pool...\n";
	tp.terminate(TERMINATION_MODE);
	std::cout << "Thread pool is terminated\n";
	if (!tp.working())
	{
		std::cout << "Thread pool is no longer working\n";
	}
	else
	{
		std::cout << "Something went worng, thread pool is still working!\n";
	}
}

void pause_resume_routine(thread_pool& tp, bool& continue_test)
{
	auto dist_resume = std::uniform_int_distribution<int>(RESUME_DELAY_MIN, RESUME_DELAY_MAX);
	auto dist_pause = std::uniform_int_distribution<int>(PAUSE_DELAY_MIN, PAUSE_DELAY_MAX);

	while (continue_test)
	{
		std::this_thread::sleep_for(std::chrono::seconds(dist_pause(mersenne)));

		if (tp.pause())
		{
			std::cout << "Thread pool is paused\n";
			std::this_thread::sleep_for(std::chrono::seconds(dist_resume(mersenne)));

			if (tp.resume())
			{
				std::cout << "Thread pool is resumed\n";
			}
			else
			{
				std::cout << "Failed to resume thread pool\n";
			}
		}
		else
		{
			std::cout << "Failed to pause thread pool\n";
		}
	}
}

void queue_size_routine(thread_pool& tp, bool& continue_test)
{
	while (continue_test)
	{
		size_t size1 = tp.get_first_queue_size();
		size_t size2 = tp.get_second_queue_size();
		printf("Queue #1: %d tasks; Queue #2: %d tasks\n", size1, size2);
		std::this_thread::sleep_for(std::chrono::seconds(QUEUE_SIZE_READ_DELAY));
	}
}

int main()
{
	thread_pool tp;
	std::cout << "Initializing thread pool...\n";
	if (tp.initialise())
	{
		std::cout << "Thread pool is initialized\n";
	}
	else
	{
		std::cout << "ERROR occured while initializng thread pool!\n";
		return 1;
	}
	if (tp.working())
	{
		std::cout << "Thread pool is working\n";
	}
	else
	{
		std::cout << "Something went wrong, thread pool is not working!\n";
	}

	bool continue_test = true;

	std::thread add_task_thread = std::thread(add_tasks_routine, std::ref(tp));
	std::this_thread::sleep_for(std::chrono::seconds(THREADS_START_DELAY));
	std::thread pause_resume_thread = std::thread(pause_resume_routine, std::ref(tp), std::ref(continue_test));
	std::thread queue_size_reader_thread = std::thread(queue_size_routine, std::ref(tp), std::ref(continue_test));

	add_task_thread.join();

	continue_test = false;

	pause_resume_thread.join();
	queue_size_reader_thread.join();

	return 0;
}