#include "UcClipboard.h"

// Qt
#include <QClipboard>
#include <QApplication>

// Libs
#include "u_Qstrings.h"

// Unicode
#include "UcData.h"
#include "UcCp.h"


void uc::copyCp(char32_t cp, CopiedChannel channel)
{
    QString q = str::toQ(cp);
    if (cp < cp::MAX_NONPRIVATE) {
        switch (channel) {
        case CopiedChannel::CHAR:
            break;
        case CopiedChannel::SAMPLE:
            if (auto ch = uc::cpsByCode[cp]) {
                if (ch->isMark()) {
                    q = uc::STUB_CIRCLE + q;
                } else if (ch->isVs16Emoji()) {
                    q += QChar(cp::VS16);
                }
            } break;
        case CopiedChannel::VS15:
            if (auto ch = uc::cpsByCode[cp]) {
                if (ch->isVs15Emoji()) {
                    q += QChar(cp::VS15);
                }
            } break;
        }
    }
    QApplication::clipboard()->setText(q);
}


bool uc::copyString(std::u32string_view sv)
{
    if (sv.empty())
        return false;
    auto q = str::toQ(sv);
    QApplication::clipboard()->setText(q);
    return true;
}


bool uc::copyNode(const uc::LibNode& node)
{
    return copyString(node.value);
}


bool uc::copyNode(const uc::LibNode* node)
{
    if (node)
        return copyNode(*node);
    return false;
}
