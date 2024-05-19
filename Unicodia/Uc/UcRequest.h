#pragma once

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

        /// @return   version is persent in query
        virtual EcVersion ecVersion() const { return EcVersion::NO_VALUE; }

        virtual bool isOk(const Cp& cp) const = 0;
        ~Request() = default;
    };

    MultiResult doRequest(const Request& rq);

    struct Fields {
        uc::EcScript ecScript = uc::EcScript::NO_VALUE;
        uc::EcVersion ecVersion = uc::EcVersion::NO_VALUE;
        uc::EcCategory ecCategory = uc::EcCategory::NO_VALUE;
    };

    class FieldRequest : public Request
    {
    public:
        FieldRequest(const Fields& x) : fields(std::move(x)) {}
        bool hasChars() const override { return true; }
        EcVersion ecVersion() const override { return fields.ecVersion; }
        bool isOk(const Cp& cp) const override;
    private:
        Fields fields;
    };

}   // namespace uc
