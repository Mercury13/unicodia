#pragma once

#include "UcSearch.h"

namespace uc {

    class Request   // interface
    {
    public:
        /// @return [+] reply may contain characters
        virtual bool hasChars() const { return true; }

        /// @return [+] reply may contain emoji
        virtual bool hasEmoji() const { return false; }

        virtual bool isOk(const Cp& cp) const = 0;
        ~Request() = default;
    };

    MultiResult doRequest(const Request& rq);

}   // namespace uc
