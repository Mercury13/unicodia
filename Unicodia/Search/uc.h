#pragma once

// Qt
#include <QString>

// Libs
#include "u_Vector.h"
#include "u_EcArray.h"

// Search
#include "defs.h"
#include "engine.h"

// Unicode
#include "UcAutoDefines.h"

namespace uc {

    struct Cp;

    DEFINE_ENUM_TYPE_IN_NS(uc, CpType, unsigned char,
        EXISTING,
        NONCHARACTER,
        PRIVATE_USE,
        SURROGATE,
        UNALLOCATED,
        RESERVED,           ///< Allocated but still reserved
        LIBNODE)            ///< library node, currently emoji
    extern const ec::Array<std::string_view, CpType> cpTypeKeys;

    DEFINE_ENUM_TYPE_IN_NS(uc, SearchError, unsigned char,
        OK,
        NO_SEARCH,          ///< Search did not occur at all
        NOT_FOUND,
        CONVERT_ERROR,
        TOO_BIG)
    extern const ec::Array<std::string_view, SearchError> searchErrorKeys;

    enum : unsigned char {
        // LOWEST
        NO_HIPRIO = 0,          ///< simple textual search, normal priority
        HIPRIO_NUMERIC,         ///< search by numeric value
        HIPRIO_NUMERIC_HI,      ///< same but stronger
        HIPRIO_MNEMONIC_CASE,   ///< search by HTML mnemonic — wrong case
        HIPRIO_MNEMONIC_EXACT,  ///< search by HTML mnemonic — exact case match
        HIPRIO_FLAG,            ///< search by flag (US etc)
        HIPRIO_DEC,             ///< decimal value matches
        HIPRIO_HEX,             ///< hex value matches
        // HIGHEST
    };

    struct MiniLine {
        char32_t code = 0xFFFFFF;           ///< char code
        CpType type = CpType::NONCHARACTER; ///< what found
        const uc::Cp* cp = nullptr;         ///< code point
        const uc::LibNode* node = nullptr;  ///< library node

        constexpr MiniLine() noexcept = default;
        // Clazy’s −warn
        constexpr MiniLine(char32_t aCode,
                 CpType aType = CpType::NONCHARACTER,
                 const uc::Cp* aCp = nullptr) noexcept
            : code(aCode), type(aType), cp(aCp) {}
        constexpr MiniLine(const uc::LibNode* aNode) : type(CpType::LIBNODE), node(aNode) {}
    };

    struct SearchLine : public MiniLine {
        std::u8string_view triggerName;     ///< name that triggered inclusion to search results
        srh::Prio prio;                     ///< its priority
        unsigned nestLevel = 0;             ///< US = U + S: US level 0, U/S level 1
        std::unique_ptr<std::u8string> ownedString {};
        /// @warning in reverse order!!
        std::partial_ordering operator <=>(const SearchLine& x) const noexcept
            { return x.prio <=> prio; }
        static const SearchLine STUB;
        SearchLine() = default;
        SearchLine(const MiniLine& x) noexcept
            : MiniLine{x} {}
        SearchLine(const uc::Cp& cp) noexcept
            : MiniLine{cp.subj, CpType::EXISTING, &cp} {}
        constexpr SearchLine(const uc::LibNode* node) noexcept
            : MiniLine{node} {}
        SearchLine(const uc::Cp& cp, std::u8string_view tn) noexcept
            : MiniLine{cp.subj, CpType::EXISTING, &cp}, triggerName(tn) {}
        SearchLine(const uc::Cp& cp, std::u8string_view tn, const srh::Prio& pr) noexcept
            : MiniLine{cp.subj, CpType::EXISTING, &cp}, triggerName(tn), prio(pr) {}
        SearchLine(const uc::LibNode* node, const srh::Prio& pr) noexcept
            : MiniLine(node), prio(pr) {}

        void giveTriggerName(std::u8string r)
        {
            ownedString = std::make_unique<std::u8string>(std::move(r));
            triggerName = *ownedString;
        }
    };

    struct SingleResult : public MiniLine {
        SearchError err = SearchError::OK;

        SingleResult(char32_t code, CpType type)
            : MiniLine{ code, type } {}
        SingleResult(char32_t code, CpType type, const uc::Cp* cp)
            : MiniLine{ code, type, cp } {}
        SingleResult(const uc::Cp& cp)
            : MiniLine{cp.subj, CpType::EXISTING, &cp} {}
        SingleResult(const MiniLine& line) : MiniLine(line) {}
        SingleResult(SearchError aErr) : err(aErr) {}
    };

    DEFINE_ENUM_TYPE_IN_NS (uc, SearchGroupObjType, unsigned char,
            NONE, BLOCK, LIBNODE)
    using SearchGroupObjFather = std::variant<std::monostate, const uc::Block*, const uc::LibNode*>;
    static_assert(std::variant_size_v<SearchGroupObjFather> == ec::size<SearchGroupObjType>());

    struct SearchGroupObj : public SearchGroupObjFather {
    public:
        using SearchGroupObjFather::SearchGroupObjFather;
        constexpr SearchGroupObjType type() const noexcept
            { return static_cast<SearchGroupObjType>(index()); }
    };

    struct SearchGroup {
        SearchGroupObj obj;
        SafeVector<SearchLine> lines;

        SearchGroup() = default;
        explicit SearchGroup(SafeVector<SearchLine>&& x) : lines(std::move(x)) {}

        size_t size() const { return lines.size(); }
        bool isEmpty() const { return lines.empty(); }
    };

    enum class ReplyStyle : unsigned char {
        FLAT,       ///< few results, have some sort of priority between
        GROUPED     ///< many results, all are equal
    };

    /// Primary object drawn in the 1st line of reply
    enum class PrimaryObj : unsigned char {
        DFLT,       ///< default (text and so on)
        NUMERIC     ///< char’s numeric meaning
    };

    struct MultiResult {
        SearchError err = SearchError::OK;
        ReplyStyle style = ReplyStyle::FLAT;
        uc::EcVersion version = uc::EcVersion::NO_VALUE;
        PrimaryObj primaryObj = PrimaryObj::DFLT;
        SafeVector<SearchGroup> groups {};

        MultiResult(ReplyStyle x, EcVersion v, PrimaryObj obj)
            : style(x), version(v), primaryObj(obj) {}
        MultiResult(const SingleResult& x);
        MultiResult(SafeVector<SearchLine>&& aV);
        const uc::Cp* one() const;
        bool isEmpty() const;
        bool hasSmth() const { return !isEmpty(); }
    };

    using DecodedEmoji = srh::Decoded<const uc::LibNode*>;

    constexpr long long NO_CODE = -1;
    SingleResult findCode(unsigned long long ull);
    SingleResult findStrCode(QStringView what, int base, long long& code);
    MultiResult doSearch(QString what);
    bool isNameChar(char32_t cp);
    bool isNameChar(QStringView x);
    bool isMnemoChar(char32_t cp);
    bool isMnemoChar(QStringView x);
    std::u8string toMnemo(const QString& x);
    void ensureEmojiSearch();
    SafeVector<DecodedEmoji> decodeEmoji(std::u32string_view s);

    /// Finds emoji (including VS16)
    /// @return  node containing x (maybe x+VS16)
    /// @warning  Lazy and unprotected → do not run in thread
    const uc::LibNode* findEmoji(char32_t x);
}
