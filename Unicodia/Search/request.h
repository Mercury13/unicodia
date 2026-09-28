#pragma once

// STL
#include <variant>

// Unicode
#include "Search/uc.h"
#include "UcData.h"

namespace uc {

    class Request   // interface
    {
    public:
        /// @return [+] reply may contain characters
        ///         [-] character version of isOk does not matter
        virtual bool hasChars() const noexcept = 0;

        /// @return [+] reply may contain emoji
        ///         [-] emoji version of isOk does not matter
        virtual bool hasEmoji() const noexcept { return false; }

        /// @return   version is persent in query
        virtual EcVersion ecVersion() const noexcept { return EcVersion::NO_VALUE; }

        virtual PrimaryObj primaryObj() const { return PrimaryObj::DFLT; }

        /// @return [+] character is within request
        virtual bool isOk(const Cp& cp) const = 0;
        /// @return [+] emoji is within request
        virtual bool isOk(const uc::LibNode&) const { return false; }
        ~Request() = default;
    };

    MultiResult doRequest(const Request& rq);

    struct Everything {};
    struct NumbersOnly {};
    using CharFields = std::variant<
            Everything, uc::EcScript, uc::EcVersion, uc::EcCategory, uc::EcUpCategory,
            uc::EcBidiClass, uc::Cfgs, uc::OldComp, uc::EgypReliability,
            uc::BreakClass, NumbersOnly>;

    class CharFieldRequest : public Request
    {
    public:
        static_assert(std::is_trivially_copy_constructible_v<CharFields>, "Something went wrong");
        template <class T>
            CharFieldRequest(const T& x) : fields(x) {}
        bool hasChars() const noexcept override { return true; }
        EcVersion ecVersion() const noexcept override;
        bool isOk(const Cp& cp) const override;
        PrimaryObj primaryObj() const override;
    private:
        CharFields fields;
    };

    struct EmojiFields {
        uc::EcVersion ecVersion = uc::EcVersion::NO_VALUE;
    };

    class EmojiFieldRequest : public Request
    {
    public:
        static_assert(std::is_trivially_copy_constructible_v<CharFields>, "Something went wrong");
        EmojiFieldRequest(const EmojiFields& x) : fields(x) {}
        // Do not search for chars…
        bool hasChars() const noexcept override { return false; }
        bool isOk(const Cp&) const override { return false; }
        // …but search for emoji
        EcVersion ecVersion() const noexcept override { return fields.ecVersion; }
        bool hasEmoji() const noexcept override { return true; }
        bool isOk(const uc::LibNode&) const override;
    private:
        EmojiFields fields;
    };

    class RqAllChars : public Request
    {
    public:
        bool hasChars() const noexcept override { return true; }
        bool isOk(const Cp& cp) const override { return true; }
        static const RqAllChars INST;
    };

}   // namespace uc
