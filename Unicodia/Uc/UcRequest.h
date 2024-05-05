#pragma once

#include "UcSearch.h"

namespace uc {

    class Request   // interface
    {
    public:
        /// @return [+] reply may contain characters
        virtual bool hasChars() const = 0;

        /// @return [+] reply may contain emoji
        virtual bool hasEmoji() const = 0;

        virtual bool isOk(const Cp& cp) const = 0;
        ~Request() = default;
    };

    MultiResult doRequest(const Request& rq);

}   // namespace uc
