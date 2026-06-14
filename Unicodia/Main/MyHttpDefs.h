#pragma once

#include <string>

//
// Workaround: under CMake, object shouldn't be half-defined!
//

namespace myht {

    enum class Error : unsigned char {
        OK,
        MISC,  // misc errors
        DID_NOT_RUN,
    };

    struct Result {
        std::string body;
        std::string errorMsg;
        int code = 0;
        Error error = Error::DID_NOT_RUN;

        constexpr operator bool() const noexcept { return (error == Error::OK); }
    };

}   // namespace myht
