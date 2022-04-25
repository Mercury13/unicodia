QT += core gui widgets svg
CONFIG += c++2a

win32-g++ {
    # To simplify debugging, we statically link these libraries
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
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
    ../Libs/SelfMade/c_WrapAroundTable.cpp \
    FmMessage.cpp \
    FmTofuStats.cpp \
    Uc/UcAuto.cpp \
    ../Libs/SelfMade/c_TableCache.cpp \
    ../Libs/SelfMade/i_MemStream.cpp \
    ../Libs/SelfMade/i_TempFont.cpp \
    ../Libs/SelfMade/u_SearchEngine.cpp \
    ../Libs/SelfMade/Strings/u_Qstrings.cpp \
    ../Libs/SelfMade/Strings/u_Strings.cpp \
    FmPopup.cpp \
    MyWiki.cpp \
    Uc/FontMatch.cpp \
    Uc/UcData.cpp \
    Uc/UcDating.cpp \
    Uc/UcSearch.cpp \
    Wiki.cpp \
    c_SearchEdit.cpp \
    main.cpp \
    FmMain.cpp \
    u_EmojiPainter.cpp

HEADERS += \
    ../Libs/L10n/LocDic.h \
    ../Libs/SelfMade/c_TableCache.h \
    ../Libs/SelfMade/c_WrapAroundTable.h \
    ../Libs/SelfMade/i_ByteSwap.h \
    ../Libs/SelfMade/i_MemStream.h \
    ../Libs/SelfMade/i_TempFont.h \
    ../Libs/SelfMade/u_Cmap.h \
    ../Libs/SelfMade/u_Iterator.h \
    ../Libs/SelfMade/u_Array.h \
    ../Libs/SelfMade/u_LruCache.h \
    ../Libs/SelfMade/u_TinyOpt.h \
    ../Libs/SelfMade/u_TypedFlags.h \
    ../Libs/SelfMade/u_Vector.h \
    ../Libs/SelfMade/u_SearchEngine.h \
    ../Libs/SelfMade/Strings/u_Qstrings.h \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Libs/Zippy/Zippy.hpp \
    FmMain.h \
    FmMessage.h \
    FmPopup.h \
    FmTofuStats.h \
    MyWiki.h \
    Skin.h \
    Uc/FontDef.h \
    Uc/FontMatch.h \
    Uc/UcAutoCount.h \
    Uc/UcAutoDefines.h \
    Uc/UcData.h \
    Uc/UcDating.h \
    Uc/UcSearch.h \
    Wiki.h \
    c_SearchEdit.h \
    u_EmojiPainter.h

FORMS += \
    FmMain.ui \
    FmTofuStats.ui

INCLUDEPATH += \
    ../Libs \
    ../Libs/L10n \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Strings \
    ../Libs/Zippy \
    Uc

LIBS += -lz

TRANSLATIONS += \
    Unicodia_en_001.ts

VERSION = 1.2.15

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
    QMAKE_TARGET_DESCRIPTION = Unicodia: character encyclopedia
    QMAKE_TARGET_COPYRIGHT =  Mikhail Merkuryev
}
