// My header
#include "Skin.h"

#include "i_DarkMode.h"

// Missing is a technical style, let it be this way
#define STYLES_WIKI_OF(popup, deprecated) \
    ".copy { " STYLE_COPY " } "                                             \
    ".altname { " STYLE_ALTNAME " } "                                       \
    ".bigcopy { " STYLE_BIGCOPY " } "                                       \
    ".popup { " popup " } "                                                 \
    ".deprecated { " deprecated " } "                                      \
    ".deph { color: " CNAME_TEXT_DEPRECATED "; } "                         \
    ".inet { " STYLE_INET " } "                                            \
    ".missing { color: " CNAME_LINK_DEPRECATED_DARK "; } "                 \
    ".tr { color: " CNAME_TRANSCRIPTION "; }"                              \
    ".query { " STYLE_QUERY " } "                                          \
    ".popdark { " STYLE_POPUP_DARK " }"

constexpr const char* STYLES_WIKI_LIGHT = STYLES_WIKI_OF(
    STYLE_POPUP_LIGHT, STYLE_DEPREC_LIGHT);

constexpr const char* STYLES_WIKI_DARK = STYLES_WIKI_OF(
    STYLE_POPUP_DARK, STYLE_DEPREC_DARK);

const char* stylesWiki()
{
    return dark::isActuallyOn() ? STYLES_WIKI_DARK : STYLES_WIKI_LIGHT;
}

const char* styleMisrender()
{
    return dark::isActuallyOn() ? STYLE_MISRENDER_DARK : STYLE_MISRENDER_LIGHT;
}

const char* stylePopup()
{
    return dark::isActuallyOn() ? STYLE_POPUP_DARK : STYLE_POPUP_LIGHT;
}
