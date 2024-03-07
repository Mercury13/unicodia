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
    if (channel == CopiedChannel::SAMPLE && cp < cp::MAX_NONPRIVATE) {
        if (auto ch = uc::cpsByCode[cp]) {
            if (ch->category().upCat == uc::UpCategory::MARK) {
                q = uc::STUB_CIRCLE + q;
            } else if (ch->isVs16Emoji()) {
                q += QChar(cp::VS16);
            }
        }
    }
    QApplication::clipboard()->setText(q);
}
