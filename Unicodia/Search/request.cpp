// My header
#include "request.h"

#include "UcOldInput.h"


const uc::RqAllChars uc::RqAllChars::INST;

namespace {

    struct EmojiState {
        uc::MultiResult& r;
        const uc::Request& rq;
        const uc::LibNode* oldCat = nullptr;
        uc::SearchGroup* lastGroup = nullptr;
    };

    void recurseEmoji(EmojiState& state, const uc::LibNode* category, unsigned index)
    {
        auto& node = uc::libNodes[index];
        if (node.flags.have(uc::Lfg::NO_COUNTING))
            return;
        if (!category && index != uc::ILIB_EMOJI_ROOT)
            category = &node;
        if (!node.value.empty() && state.rq.isOk(node)) {
            // Found!
            if (category != state.oldCat) {
                state.lastGroup = &state.r.groups.emplace_back();
                state.lastGroup->obj = category;
                state.oldCat = category;
            }
            if (state.lastGroup) {  // -warn, should always be true
                state.lastGroup->lines.emplace_back(&node, srh::EmojiLevel::FULL);
            }
        }
        const auto a = node.iFirstChild;
        const auto b = a + node.nChildren;
        for (int i = a; i < b; ++i)
            recurseEmoji(state, category, i);
    }

}   // anon namespace

uc::MultiResult uc::doRequest(const Request& rq)
{
    uc::MultiResult r(uc::ReplyStyle::GROUPED, rq.ecVersion(), rq.primaryObj());

    if (rq.hasChars()) {
        const uc::Block* oldBlock = nullptr;
        uc::SearchGroup* lastGroup = nullptr;
        for (const auto& cp : uc::cpInfo) {
            if (rq.isOk(cp)) {
                auto* newBlock = &cp.block();
                if (newBlock != oldBlock) {
                    lastGroup = &r.groups.emplace_back();
                    lastGroup->obj = newBlock;
                    oldBlock = newBlock;
                }
                lastGroup->lines.emplace_back(cp);
            }
        }
    }

    if (rq.hasEmoji()) {
        EmojiState state { .r = r, .rq = rq };
        recurseEmoji(state, nullptr, ILIB_EMOJI_ROOT);
    }

    return r;
}


uc::MultiResult uc::doDosAltRequest(uc::DosLang rq)
{
    uc::MultiResult r(uc::ReplyStyle::FLAT, uc::EcVersion::NO_VALUE,
                      uc::PrimaryObj::DFLT);
    auto& lastGroup = r.groups.emplace_back();
    const auto& myInfo = uc::oneByteInfo(rq);
    for (unsigned c = 1; c <= 255; ++c) {
        if (char16_t ch = uc::dosAltDecode(myInfo, c)) {
            if (auto cp = uc::cpsByCode[ch]) {
                lastGroup.lines.emplace_back(*cp);
            }
        }
    }
    return r;
}


uc::MultiResult uc::doWinAltRequest(uc::WinLang rq)
{
    uc::MultiResult r(uc::ReplyStyle::FLAT, uc::EcVersion::NO_VALUE,
                      uc::PrimaryObj::DFLT);
    auto& lastGroup = r.groups.emplace_back();
    const auto& myInfo = uc::oneByteInfo(rq);
    for (unsigned c = 1; c <= 255; ++c) {
        if (char16_t ch = uc::winAltDecode(myInfo, c)) {
            if (auto cp = uc::cpsByCode[ch]) {
                lastGroup.lines.emplace_back(*cp);
            }
        }
    }
    return r;
}


///// CharFieldRequest /////////////////////////////////////////////////////////


namespace {

    /// @return
    ///    [-] NO_VALUE in fields structure (OK), or really equal (OK too)
    ///    [+] the fields are really inequal
    template <class Ec>
    inline bool isIneq(Ec inFields, Ec inCp)
        { return (inFields != Ec::NO_VALUE && inFields != inCp); }

}   // anon namespace


uc::EcVersion uc::CharFieldRequest::ecVersion() const noexcept
{
    auto ptr = std::get_if<uc::EcVersion>(&fields);
    return ptr ? *ptr : uc::EcVersion::NO_VALUE;
}


namespace {

    class CharFieldsVisitor
    {
    public:
        explicit CharFieldsVisitor(const uc::Cp& aCp) noexcept : fCp(aCp) {}
        bool operator () (uc::Everything) const noexcept { return true; }
        bool operator () (uc::EcScript x) const noexcept { return (fCp.ecScript == x); }
        bool operator () (uc::EcVersion x) const noexcept { return (fCp.ecVersion == x); }
        bool operator () (uc::EcCategory x) const noexcept { return (fCp.ecCategory == x); }
        bool operator () (uc::EcUpCategory x) const noexcept { return (fCp.category().upCat == x); }
        bool operator () (uc::EcBidiClass x) const noexcept { return (fCp.ecBidiClass == x); }
        bool operator () (uc::Cfgs x) const noexcept { return fCp.flags.haveAny(x); }
        bool operator () (uc::OldComp x) const noexcept { return uc::cpOldComps(fCp.subj).have(x); }
        bool operator () (uc::EgypReliability x) const noexcept
        {
            return (fCp.script().scriptSpec == uc::ScriptSpec::RELIABILITY_EGYP
                   && fCp.scriptSpecific.egypReliability() == x);
        }
        bool operator () (uc::BreakClass x) const noexcept { return (fCp.breakClass == x); }
        bool operator () (uc::NumbersOnly) const noexcept { return fCp.numeric().isPresent(); }
    private:
        const uc::Cp& fCp;
    };

}   // anon namespace


bool uc::CharFieldRequest::isOk(const Cp& cp) const
{
    return std::visit<bool>(CharFieldsVisitor(cp), fields);
}


namespace {

    struct NumberChecker
    {
        bool operator () (uc::NumbersOnly) const noexcept { return true; }
        bool operator () (uc::EcCategory x) const noexcept
            { return (x >= uc::EcCategory::NUMBER_FIRST && x <= uc::EcCategory::NUMBER_LAST); }
        bool operator () (uc::EcUpCategory x) const noexcept
            { return x == uc::EcUpCategory::NUMBER; }
        template <class T> bool operator () (const T&) const noexcept { return false; }
    };

}   // anon namespace


uc::PrimaryObj uc::CharFieldRequest::primaryObj() const
{
    // Checks for future remakes
    static_assert((int)uc::PrimaryObj::DFLT == (int)false);
    static_assert((int)uc::PrimaryObj::NUMERIC == (int)true);
    // The most optimized version for this checks
    return static_cast<uc::PrimaryObj>(
        std::visit(NumberChecker{}, fields));
}


///// EmojiFieldRequest ////////////////////////////////////////////////////////


bool uc::EmojiFieldRequest::isOk(const uc::LibNode& node) const
{
    // Version
    if (isIneq(fields.ecVersion, node.ecEmojiVersion))
        return false;
    return true;
}
