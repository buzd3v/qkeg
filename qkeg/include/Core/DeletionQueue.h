#pragma once

#include <deque>
#include <functional>
class DeletionQueue
{
  public:
    void pushFunction(std::function<void()> &&function) { deletors.push_back(function); }
    void flush()
    {
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
        {
            (*it)();
        }

        deletors.clear();
    }

  private:
    std::deque<std::function<void()>> deletors;
};