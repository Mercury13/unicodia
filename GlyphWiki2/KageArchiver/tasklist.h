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
#include "uc.h"


DEFINE_EXCEPTION_CLASS(BadTask, std::logic_error)


struct TaskSets
{
    struct Country {
        bool isAggressive = false;
        bool limitToUnihan = false;
        SafeVector<std::string> suffixSequence;
    } country;
};


struct Candidate {
    std::string text;
    bool isFallback;
};


struct Task
{
    std::shared_ptr<TaskSets> sets;
    bool alertWhenTrueCountry = false;

    [[nodiscard]] SafeVector<Candidate> candidates(
            char32_t code, const UnicodeList& ulist) const;
};

using TaskList = std::map<char32_t, Task>;

[[nodiscard]] TaskList readTaskList();
