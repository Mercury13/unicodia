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
        bool isAggressive = false;  ///< [+] aggressive search for country-specific variants
        bool limitToUnihan = false; ///< [+] limit to options actually present in UniHan base
        bool orErase = false;       ///< [+] erase if lookup failed
        SafeVector<std::string> suffixSequence;
    } country;
    std::string prefix = "u";
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
