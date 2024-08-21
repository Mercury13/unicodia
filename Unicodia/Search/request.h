#pragma once

// Unicode
#include "Search/uc.h"
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

        virtual PrimaryObj primaryObj() const { return PrimaryObj::DFLT; }

        virtual bool isOk(const Cp& cp) const = 0;
        ~Request() = default;
    };

    MultiResult doRequest(const Request& rq);

    struct Fields {
        uc::EcScript ecScript = uc::EcScript::NO_VALUE;
        uc::EcVersion ecVersion = uc::EcVersion::NO_VALUE;        
        uc::EcCategory ecCategory = uc::EcCategory::NO_VALUE;
        uc::EcUpCategory ecUpCat = uc::EcUpCategory::NO_VALUE;
        uc::EcBidiClass ecBidiClass = uc::EcBidiClass::NO_VALUE;
        uc::Cfgs fgs {};
        bool isNumber = false;
    };

    class FieldRequest : public Request
    {
    public:
        static_assert(std::is_trivially_copy_constructible_v<Fields>, "Check ctors and choose the most");
        FieldRequest(const Fields& x) : fields(x) {}
        bool hasChars() const override { return true; }
        EcVersion ecVersion() const override { return fields.ecVersion; }
        bool isOk(const Cp& cp) const override;
        PrimaryObj primaryObj() const override;
    private:
        Fields fields;
    };

}   // namespace uc
