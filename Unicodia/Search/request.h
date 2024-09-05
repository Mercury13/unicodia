#pragma once

// Unicode
#include "Search/uc.h"
#include "UcData.h"

namespace uc {

    class Request   // interface
    {
    public:
        /// @return [+] reply may contain characters
        ///         [-] character version of isOk does not matter
        virtual bool hasChars() const = 0;

        /// @return [+] reply may contain emoji
        ///         [-] emoji version of isOk does not matter
        virtual bool hasEmoji() const { return false; }

        /// @return   version is persent in query
        virtual EcVersion ecVersion() const { return EcVersion::NO_VALUE; }

        virtual PrimaryObj primaryObj() const { return PrimaryObj::DFLT; }

        /// @return [+] character is within request
        virtual bool isOk(const Cp& cp) const = 0;
        /// @return [+] emoji is within request
        virtual bool isOk(const uc::LibNode& node) const = 0;
        ~Request() = default;
    };

    MultiResult doRequest(const Request& rq);

    struct CharFields {
        uc::EcScript ecScript = uc::EcScript::NO_VALUE;
        uc::EcVersion ecVersion = uc::EcVersion::NO_VALUE;        
        uc::EcCategory ecCategory = uc::EcCategory::NO_VALUE;
        uc::EcUpCategory ecUpCat = uc::EcUpCategory::NO_VALUE;
        uc::EcBidiClass ecBidiClass = uc::EcBidiClass::NO_VALUE;
        uc::Cfgs fgs {};
        bool isNumber = false;
    };

    class CharFieldRequest : public Request
    {
    public:
        static_assert(std::is_trivially_copy_constructible_v<CharFields>, "Something went wrong");
        CharFieldRequest(const CharFields& x) : fields(x) {}
        bool hasChars() const override { return true; }
        EcVersion ecVersion() const override { return fields.ecVersion; }
        bool isOk(const Cp& cp) const override;
        bool isOk(const uc::LibNode&) const override { return false; }
        PrimaryObj primaryObj() const override;
    private:
        CharFields fields;
    };

}   // namespace uc
