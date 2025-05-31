// My header
#include "request.h"

#include "UcOldInput.h"

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
                state.lastGroup->lines.emplace_back(&node);
            }
        }
        const auto a = node.iFirstChild;
        const auto b = a + node.nChildren;
        for (short i = a; i < b; ++i)
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


///// CharFieldRequest /////////////////////////////////////////////////////////


namespace {

    template <class Ec>
    inline bool isIneq(Ec inFields, Ec inCp)
        { return (inFields != Ec::NO_VALUE && inFields != inCp); }

}   // anon namespace


bool uc::CharFieldRequest::isOk(const Cp& cp) const
{
    // Version
    if (isIneq(fields.ecVersion, cp.ecVersion))
        return false;
    // Script
    if (isIneq(fields.ecScript, cp.ecScript))
        return false;
    // Category / up-category
    if (fields.ecCategory != EcCategory::NO_VALUE) {
        if (fields.ecCategory != cp.ecCategory)
            return false;
    } else if (fields.ecUpCat != EcUpCategory::NO_VALUE) {
        if (fields.ecUpCat != cp.category().upCat)
            return false;
    }
    // Bidirectional class
    if (isIneq(fields.ecBidiClass, cp.ecBidiClass))
        return false;
    // Flags
    if (fields.fgs && !cp.flags.haveAny(fields.fgs))
        return false;
    // Number
    if (fields.isNumber && !cp.numeric().isPresent())
        return false;
    // Old computers
    if (fields.oldComp != uc::OldComp::NONE && !uc::cpOldComps(cp.subj).have(fields.oldComp))
        return false;
    if (fields.egypReliability != uc::EgypReliability::DUMMY
            && (cp.script().scriptSpec != ScriptSpec::RELIABILITY_EGYP
                || cp.scriptSpecific.egypReliability() != fields.egypReliability))
        return false;
    return true;
}


uc::PrimaryObj uc::CharFieldRequest::primaryObj() const
{
    // Checks for future remakes
    static_assert((int)uc::PrimaryObj::DFLT == (int)false);
    static_assert((int)uc::PrimaryObj::NUMERIC == (int)true);
    // The most optimized version for this checks
    return static_cast<uc::PrimaryObj>(fields.isNumber);
}


///// EmojiFieldRequest ////////////////////////////////////////////////////////


bool uc::EmojiFieldRequest::isOk(const uc::LibNode& node) const
{
    // Version
    if (isIneq(fields.ecVersion, node.ecEmojiVersion))
        return false;
    return true;
}
