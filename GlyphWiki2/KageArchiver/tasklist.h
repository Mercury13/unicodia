#pragma once

// C++
#include <stdexcept>

// STL
#include <map>

// Libs
#include "u_Vector.h"

// Project-local
#include "excep.h"


DEFINE_EXCEPTION_CLASS(BadData, std::logic_error)
DEFINE_EXCEPTION_CLASS(BadTask, std::logic_error)


struct Task
{
    // No data for now, but next time we’ll have Taiwan
    [[nodiscard]] SafeVector<std::string> candidates(char32_t code) const;
};

using TaskList = std::map<char32_t, Task>;

[[nodiscard]] TaskList readTaskList();
