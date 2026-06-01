// My header
#include "Skin.h"

#include "i_DarkMode.h"

// Light = light PALETTE rather than light colour
// Color names
#define CNAME_BG_POPUP_LIGHT "LightYellow"
#define CNAME_BG_POPUP_DARK "#1E3333"   // DarkSlateGray * 2/3
#define CNAME_BORDER_POPUP_LIGHT "black"
#define CNAME_BORDER_POPUP_DARK "SlateGray"
#define CNAME_LINK_POPUP_LIGHT "ForestGreen"
#define CNAME_LINK_POPUP_DARK "MediumSpringGreen"
#define CNAME_LINK_POPUP_LO_LIGHT "DarkGreen"
#define CNAME_LINK_POPUP_LO_DARK "#BBFFBB"
#define CNAME_LINK_DEPRECATED_LIGHT "#CC0000"
#define CNAME_LINK_DEPRECATED_DARK  "#EF2929"
#define CNAME_ALTNAME_LIGHT "#808080"
#define CNAME_ALTNAME_DARK  "#A0A0A0"
#define CNAME_TEXT_DEPRECATED_LIGHT CNAME_ALTNAME_LIGHT
#define CNAME_TEXT_DEPRECATED_DARK  CNAME_ALTNAME_DARK
#define CNAME_LINK_OUTSIDE_LIGHT "#1565C0"        // Google blue 800
#define CNAME_LINK_OUTSIDE_DARK "DeepSkyBlue"

#define STYLE_POPUP_LIGHT  STYLE_LINK(CNAME_LINK_POPUP_LIGHT, "0.05")
#define STYLE_POPUP_DARK   STYLE_LINK(CNAME_LINK_POPUP_DARK,  "0.04")
#define STYLE_POPUP_LO_LIGHT STYLE_LINK(CNAME_LINK_POPUP_LO_LIGHT, "0.04")
#define STYLE_POPUP_LO_DARK  STYLE_LINK(CNAME_LINK_POPUP_LO_DARK,  "0.04")
#define STYLE_DEPREC_LIGHT STYLE_LINK(CNAME_LINK_DEPRECATED_LIGHT, "0.04")
#define STYLE_DEPREC_DARK  STYLE_LINK(CNAME_LINK_DEPRECATED_DARK, "0.04")
#define STYLE_ALTNAME_OF(color, ratio) STYLE_LINK2(color, CNAME_U_COPY, ratio) " font-weight:bold;"
#define STYLE_ALTNAME_LIGHT STYLE_ALTNAME_OF(CNAME_ALTNAME_LIGHT, "0.04")
#define STYLE_ALTNAME_DARK  STYLE_ALTNAME_OF(CNAME_ALTNAME_DARK, "0.04")
#define STYLE_MISRENDER_LIGHT "color:" CNAME_LINK_DEPRECATED_LIGHT
#define STYLE_MISRENDER_DARK  "color:" CNAME_LINK_DEPRECATED_DARK
#define STYLE_INET_LIGHT    STYLE_LINK(CNAME_LINK_OUTSIDE_LIGHT, "0.05")
#define STYLE_INET_DARK     STYLE_LINK(CNAME_LINK_OUTSIDE_DARK,  "0.05")
#define STYLE_BIGINET_LIGHT STYLE_LINK2(CNAME_LINK_OUTSIDE_LIGHT, CNAME_U_BIGOUTSIDE, "0.05")
#define STYLE_BIGINET_DARK  STYLE_LINK2(CNAME_LINK_OUTSIDE_DARK,  CNAME_U_BIGOUTSIDE, "0.05")
#define STYLE_QUERY_LIGHT   "color:" CNAME_LINK_OUTSIDE_LIGHT "; text-decoration:none;"
#define STYLE_QUERY_DARK    "color:" CNAME_LINK_OUTSIDE_DARK  "; text-decoration:none;"


// Missing is a technical style, let it be this way
#define STYLES_WIKI_OF(popup, deprRed, deprGray, altname, \
        popupLo, inet, query) \
    ".copy { " STYLE_COPY " } "                               \
    ".altname { " altname " } "                               \
    ".bigcopy { " STYLE_BIGCOPY " } "                         \
    ".popup { " popup " } "                                   \
    ".deprecated { " deprRed " } "                            \
    ".deph, .altname2 { color: " deprGray "; } "              \
    ".inet { " inet " } "                                     \
    ".missing { color: " CNAME_LINK_DEPRECATED_DARK "; } "    \
    ".tr { color: " CNAME_TRANSCRIPTION "; }"                 \
    ".query { " query " } "                                   \
    ".popdark { " popupLo " }"

constexpr const char* STYLES_WIKI_LIGHT = STYLES_WIKI_OF(
    STYLE_POPUP_LIGHT, STYLE_DEPREC_LIGHT, CNAME_ALTNAME_LIGHT,
    STYLE_ALTNAME_LIGHT, STYLE_POPUP_LO_LIGHT, STYLE_INET_LIGHT,
    STYLE_QUERY_LIGHT);

constexpr const char* STYLES_WIKI_DARK = STYLES_WIKI_OF(
    STYLE_POPUP_DARK, STYLE_DEPREC_DARK, CNAME_ALTNAME_DARK,
    STYLE_ALTNAME_DARK, STYLE_POPUP_LO_DARK, STYLE_INET_DARK,
    STYLE_QUERY_DARK);

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

const char* stylePopupLo()
{
    return dark::isActuallyOn() ? STYLE_POPUP_LO_DARK : STYLE_POPUP_LO_LIGHT;
}

const char* cnameBgPopup()
{
    return dark::isActuallyOn() ? CNAME_BG_POPUP_DARK : CNAME_BG_POPUP_LIGHT;
}

const char* cnameBorderPopup()
{
    return dark::isActuallyOn() ? CNAME_BORDER_POPUP_DARK : CNAME_BORDER_POPUP_LIGHT;
}

const char* styleInet()
{
    return dark::isActuallyOn() ? STYLE_INET_DARK : STYLE_INET_LIGHT;
}

const char* styleBigInet()
{
    return dark::isActuallyOn() ? STYLE_BIGINET_DARK : STYLE_BIGINET_LIGHT;
}

const char* cnameInet()
{
    return dark::isActuallyOn() ? CNAME_LINK_OUTSIDE_DARK : CNAME_LINK_OUTSIDE_LIGHT;
}
