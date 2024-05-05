#pragma once

// STL
#include <optional>

// Unicode
#include "UcSearch.h"
#include "UcData.h"

namespace uc {

    class Request   // interface
    {
    public:
        /// @return [+] reply may contain characters
        virtual bool hasChars() const = 0;

        /// @return [+] reply may contain emoji
        virtual bool hasEmoji() const { return false; }

        virtual bool isOk(const Cp& cp) const = 0;
        ~Request() = default;
    };

    MultiResult doRequest(const Request& rq);

    struct Fields {
        uc::EcVersion version = uc::EcVersion::NONE;
    };

    class FieldRequest : public Request
    {
    public:
        FieldRequest(const Fields& x) : fields(std::move(x)) {}
        bool hasChars() const override { return true; }
        bool isOk(const Cp& cp) const override;
    private:
        Fields fields;
    };

}   // namespace uc
