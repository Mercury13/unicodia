#pragma once

#include <QTranslator>

#include "u_Vector.h"

namespace loc
{

    struct Lang
    {
        struct Name {
            std::u8string
                tech,           ///< Chinese
                native,         ///< Chinese (simplified)
                international,  ///< 简体中文
                isoSmall,       ///< zh
                isoBig;         ///< zh-cn
        } name;
        bool showEnglishTerms = true;
        std::unique_ptr<QTranslator> fTranslator;
    };

    using LangList = SafeVector<Lang>;

    LangList allLangs;

    void collectLangs();

}   // namespace loc
