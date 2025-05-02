#pragma once

// C++
#include <stdexcept>

// STL
#include <map>
#include <memory>

// Libs
#include "u_Vector.h"

// Project-local
#include "excep.h"


DEFINE_EXCEPTION_CLASS(BadTask, std::logic_error)


struct TaskSets
{
    struct Country {
        bool isAggressive = false;
        SafeVector<std::string> suffixSequence;
    } country;
};


struct Task
{
    std::shared_ptr<TaskSets> sets;

    [[nodiscard]] SafeVector<std::string> candidates(char32_t code) const;
};

using TaskList = std::map<char32_t, Task>;

[[nodiscard]] TaskList readTaskList();

#define PRF_SV(x) (unsigned((x).length())), ((x).data())
