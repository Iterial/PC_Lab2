//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <chrono>
#include <thread>

#include "thread_pool.h"

#define MIN_TIME 1
#define MAX_TIME 1

static class task_generator
{
public:
	static task generate_task()
	{
		task t(rand() % (MAX_TIME - MIN_TIME + 1) + MIN_TIME);
		return t;
	}

	static void wait()
	{
		unsigned int time_to_sleep = rand() % MAX_TIME + (MIN_TIME - MIN_TIME / 2);
		std::this_thread::sleep_for(std::chrono::seconds(time_to_sleep));
	}

	static void test()
	{
		srand(time(NULL));

		thread_pool tp;
		tp.initialise();

		printf("s-");
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		tp.add_task(generate_task());
		printf("-f-");

		std::this_thread::sleep_for(std::chrono::seconds(5));
		tp.pause();
		printf("-p-");
		std::this_thread::sleep_for(std::chrono::seconds(5));
		tp.resume();
		printf("-r-");
		std::this_thread::sleep_for(std::chrono::seconds(20));

		//tp.pause();
		//tp.resume();
		//tp.add_task(generate_task());
		//wait();

		tp.terminate();

		printf("-term");
	}
};