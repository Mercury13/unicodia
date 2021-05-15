QT += core gui widgets
CONFIG += c++17

win32-g++ {
    # Some Qt versions have this thing
    QMAKE_CXXFLAGS += -Wno-deprecated-copy
    # To simplify debugging, we statically link these libraries
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

debug {
    DEFINES += AT_RANGE_CHECK
}

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Libs/SelfMade/u_Qstrings.cpp \
    ../Libs/SelfMade/u_Strings.cpp \
    Uc/UcAuto.cpp \
    Uc/UcData.cpp \
    main.cpp \
    FmMain.cpp

HEADERS += \
    ../Libs/SelfMade/u_Qstrings.h \
    ../Libs/SelfMade/u_Array.h \
    ../Libs/SelfMade/u_Strings.h \
    ../Libs/SelfMade/u_Vector.h \
    FmMain.h \
    Uc/UcData.h \
    Uc/UcDefines.h

FORMS += \
    FmMain.ui

INCLUDEPATH += \
    ../Libs/SelfMade \
    Uc

TRANSLATIONS += \
    Unicodia_en_001.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
