#include "UcClipboard.h"

// Qt
#include <QClipboard>
#include <QApplication>

// Libs
#include "u_Qstrings.h"

// Unicode
#include "UcData.h"
#include "UcCp.h"


namespace {

    void tryAddVs(QString& q, char32_t cp, QChar vs)
    {
        if (auto ch = uc::cpsByCode[cp]) {
            if (ch->isEmoji()) {
                q += vs;
            }
        }
    }

}


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
        case CopiedChannel::HARD_VS15:
            tryAddVs(q, cp, QChar(cp::VS15));
            break;
        case CopiedChannel::HARD_VS16:
            tryAddVs(q, cp, QChar(cp::VS16));
            break;
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
