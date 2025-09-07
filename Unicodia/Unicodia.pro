QT += core gui widgets network svg svgwidgets
CONFIG += c++2a c++20

win32-g++ {
    # To simplify debugging, we statically link these libraries
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
    # Qt — system headers
    QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_STRINGS

CONFIG(debug, debug|release) {
    DEFINES += AT_RANGE_CHECK
}

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Libs/L10n/LocDic.cpp \
    ../Libs/L10n/LocFmt.cpp \
    ../Libs/L10n/LocManager.cpp \
    ../Libs/L10n/LocQt.cpp \
    ../Libs/PugiXml/pugixml.cpp \
    ../Libs/SelfMade/Fonts/MemFont.cpp \
    ../Libs/SelfMade/GitHub/parsers.cpp \
    ../Libs/SelfMade/Mojibake/cpp/auto_casefold.cpp \
    ../Libs/SelfMade/Qt/QtMultiRadio.cpp \
    ../Libs/SelfMade/Qt/RememberWindow.cpp \
    ../Libs/SelfMade/c_WrapAroundTable.cpp \
    ../Libs/SelfMade/i_DarkMode.cpp \
    ../Libs/SelfMade/u_Version.cpp \
    CharPaint/SkinToneQa.cpp \
    CharPaint/routines.cpp \
    CharPaint/IconEngines.cpp \
    CharPaint/emoji.cpp \
    FmFontSupport.cpp \
    FmPopupChar.cpp \
    Main/FmMain.cpp \
    Main/MainGui.cpp \
    Main/WiLibCp.cpp \
    Main/WiOsStyle.cpp \
    Main/WiSample.cpp \
    Main/WiShowcase.cpp \
    Main/tables.cpp \
    Search/engine.cpp \
    Search/nonAscii.cpp \
    Search/request.cpp \
    Search/uc.cpp \
    FmMessage.cpp \
    FmTofuStats.cpp \
    LocList.cpp \
    Uc/UcAuto.cpp \
    ../Libs/SelfMade/c_TableCache.cpp \
    ../Libs/SelfMade/Fonts/TempFont.cpp \
    ../Libs/SelfMade/i_MemStream.cpp \
    ../Libs/SelfMade/Strings/u_Decoders.cpp \
    ../Libs/SelfMade/Strings/u_Qstrings.cpp \
    ../Libs/SelfMade/Strings/u_Strings.cpp \
    FmPopup.cpp \
    MyWiki.cpp \
    Uc/FontMatch.cpp \
    Uc/UcAutoLib.cpp \
    Uc/UcAutoOldComp.cpp \
    Uc/UcAutoSutton.cpp \
    Uc/UcBlocks.cpp \
    Uc/UcClipboard.cpp \
    Uc/UcCountries.cpp \
    Uc/UcData.cpp \
    Uc/UcDating.cpp \
    Uc/UcFonts.cpp \
    Uc/UcOldInput.cpp \
    Uc/UcScripts.cpp \
    Wiki.cpp \
    c_SearchEdit.cpp \
    cmdline.cpp \
    d_Config.cpp \
    main.cpp \
    qa.cpp \
    u_Popup.cpp

HEADERS += \
    ../Libs/L10n/LocDic.h \
    ../Libs/L10n/LocFmt.h \
    ../Libs/L10n/LocManager.h \
    ../Libs/L10n/LocQt.h \
    ../Libs/PugiXml/pugiconfig.hpp \
    ../Libs/PugiXml/pugixml.hpp \
    ../Libs/SelfMade/Fonts/MemFont.h \
    ../Libs/SelfMade/Fonts/TempFont.h \
    ../Libs/SelfMade/GitHub/parsers.h \
    ../Libs/SelfMade/Mojibake/mojibake.h \
    ../Libs/SelfMade/Qt/QtMultiRadio.h \
    ../Libs/SelfMade/Qt/RememberWindow.h \
    ../Libs/SelfMade/c_TableCache.h \
    ../Libs/SelfMade/c_WrapAroundTable.h \
    ../Libs/SelfMade/i_ByteSwap.h \
    ../Libs/SelfMade/i_DarkMode.h \
    ../Libs/SelfMade/i_MemStream.h \
    ../Libs/SelfMade/u_Cmap.h \
    ../Libs/SelfMade/u_DumbSp.h \
    ../Libs/SelfMade/u_EcArray.h \
    ../Libs/SelfMade/u_Iterator.h \
    ../Libs/SelfMade/u_Array.h \
    ../Libs/SelfMade/u_LruCache.h \
    ../Libs/SelfMade/u_MicroList.h \
    ../Libs/SelfMade/u_TinyOpt.h \
    ../Libs/SelfMade/u_TypedFlags.h \
    ../Libs/SelfMade/u_Vector.h \
    ../Libs/SelfMade/Strings/u_Decoders.h \
    ../Libs/SelfMade/Strings/u_Qstrings.h \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Libs/SelfMade/u_Version.h \
    ../Libs/Zippy/Zippy.hpp \
    CharPaint/SkinToneQa.h \
    CharPaint/routines.h \
    CharPaint/IconEngines.h \
    CharPaint/global.h \
    CharPaint/emoji.h \
    FmFontSupport.h \
    FmPopupChar.h \
    LocDefs.h \
    Main/FmMain.h \
    Main/MainGui.h \
    Main/tables.h \
    Main/WiLibCp.h \
    Main/WiOsStyle.h \
    Main/WiSample.h \
    Main/WiShowcase.h \
    Search/defs.h \
    Search/engine.h \
    Search/nonAscii.h \
    Search/request.h \
    Search/trie.h \
    Search/uc.h \
    FmMessage.h \
    FmPopup.h \
    FmTofuStats.h \
    LocList.h \
    MyWiki.h \
    Skin.h \
    Uc/FontDef.h \
    Uc/FontMatch.h \
    Uc/UcAutoCount.h \
    Uc/UcAutoDefines.h \
    Uc/UcAutoScripts.h \
    Uc/UcClipboard.h \
    Uc/UcCommon.h \
    Uc/UcContinents.h \
    Uc/UcCp.h \
    Uc/UcData.h \
    Uc/UcDating.h \
    Uc/UcFlags.h \
    Uc/UcOldInput.h \
    Uc/UcRecent.h \
    Uc/UcSkin.h \
    Wiki.h \
    c_SearchEdit.h \
    cmdline.h \
    d_Config.h \
    qa.h \
    u_Popup.h

FORMS += \
    FmFontSupport.ui \
    FmPopupChar.ui \
    Main/FmMain.ui \
    Main/WiLibCp.ui \
    Main/WiOsStyle.ui \
    Main/WiSample.ui \
    Main/WiShowcase.ui \
    FmTofuStats.ui

INCLUDEPATH += \
    ../Libs \
    ../Libs/L10n \
    ../Libs/MagicEnum \
    ../Libs/PugiXml \
    ../Libs/RapidJson \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Mojibake \
    ../Libs/SelfMade/Qt \
    ../Libs/SelfMade/Strings \
    ../Libs/Zippy \
	Main \
    Uc

LIBS += -lz

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources/Scripts/scripts.qrc \
    Resources/res.qrc

win32|win64 {
    RC_ICONS = Unicodia-win.ico
    QMAKE_TARGET_COMPANY = Mikhail Merkuryev
    QMAKE_TARGET_PRODUCT = Unicodia
    QMAKE_TARGET_DESCRIPTION = Unicodia: encyclopedia of characters
    QMAKE_TARGET_COPYRIGHT =  Mikhail Merkuryev
}

VERSION_FILE = ../VERSION
VERSION = $$cat($${VERSION_FILE})
{ # To trigger QMAKE
    include($${VERSION_FILE})
}
