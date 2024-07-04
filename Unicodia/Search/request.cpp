// My header
#include "request.h"

uc::MultiResult uc::doRequest(const Request& rq)
{
    uc::MultiResult r(uc::ReplyStyle::GROUPED, rq.ecVersion());

    const uc::Block* oldBlock = nullptr;
    uc::SearchGroup* lastGroup = nullptr;
    if (rq.hasChars()) {
        for (const auto& cp : uc::cpInfo) {
            if (rq.isOk(cp)) {
                auto* newBlock = &cp.block();
                if (newBlock != oldBlock) {
                    lastGroup = &r.groups.emplace_back();
                    lastGroup->block = newBlock;
                    oldBlock = newBlock;
                }
                lastGroup->lines.emplace_back(cp);
            }
        }
    }

    if (rq.hasEmoji()) {
        /// @todo [future] search for emoji
    }

    return r;
}


///// FieldRequest /////////////////////////////////////////////////////////////


namespace {

    template <class Ec>
    inline bool isIneq(Ec inFields, Ec inCp)
        { return (inFields != Ec::NO_VALUE && inFields != inCp); }

}   // anon namespace


bool uc::FieldRequest::isOk(const Cp& cp) const
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
    return true;
}
