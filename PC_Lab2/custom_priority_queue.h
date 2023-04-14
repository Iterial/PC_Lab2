#pragma once

#include <queue>
#include <stdexcept>

#include "task.h"

class custom_priority_queue : public std::priority_queue<task, std::vector<task>, std::greater<task>>
{
private:
    using std::priority_queue<task, std::vector<task>, std::greater<task>>::priority_queue;

public:
    bool remove_tired(std::vector<task>& tired_tasks);

private:
    bool remove(const task& value);
};