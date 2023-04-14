#include "custom_priority_queue.h"

bool custom_priority_queue::remove_tired(std::vector<task>& tired_tasks)
{
    for (const auto& t : c)
    {
        if (t.is_tired())
        {
            tired_tasks.push_back(t);
        }
    }

    for (const auto& t : tired_tasks)
    {
        if (!remove(t))
        {
            throw std::logic_error("Failed to delete previously found task!");
        }
    }

    return !tired_tasks.empty();
}

bool custom_priority_queue::remove(const task& value)
{
    auto it = c.begin();
    for (; it < c.end(); it++)
    {
        if (*it == value)
        {
            break;
        }
    }

    if (it == c.end())
    {
        return false;
    }

    if (it == c.begin())
    {
        pop();
    }
    else
    {
        c.erase(it);
        std::make_heap(c.begin(), c.end(), comp);
    }

    return true;
}