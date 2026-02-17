QT       += core gui widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Libs/SelfMade/Fonts/MemFont.cpp \
    ../Libs/SelfMade/Fonts/MemWhatChanged.cpp \
    ../Libs/SelfMade/Qt/i_OpenSave.cpp \
    ../Libs/SelfMade/Qt/u_OpenSaveStrings.cpp \
    ../Libs/SelfMade/Strings/u_Strings.cpp \
    ../Libs/SelfMade/i_MemStream.cpp \
    main.cpp \
    FmMain.cpp

HEADERS += \
    ../Libs/SelfMade/Fonts/MemFont.h \
    ../Libs/SelfMade/Fonts/MemFontDefs.h \
    ../Libs/SelfMade/Fonts/MemWhatChanged.h \
    ../Libs/SelfMade/Qt/i_OpenSave.h \
    ../Libs/SelfMade/Qt/u_OpenSaveStrings.h \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Libs/SelfMade/i_MemStream.h \
    ../Libs/SelfMade/u_Array.h \
    ../Libs/function_ref.hpp \
    FmMain.h

INCLUDEPATH += \
    ../Libs \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Fonts \
    ../Libs/SelfMade/Qt \
    ../Libs/SelfMade/Strings

FORMS += \
    FmMain.ui

win32-g++ {
    # To simplify debugging, we statically link these libraries
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
    # Qt — system headers
    QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]
}

win32 {
    LIBS += -lcomdlg32
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
