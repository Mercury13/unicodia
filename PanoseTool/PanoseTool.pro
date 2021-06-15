QT += core gui widgets
CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32-g++ {
    # To simplify debugging, we statically link these libraries
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
}

SOURCES += \
    ../Libs/SelfMade/u_Qstrings.cpp \
    FileWrap.cpp \
    Ttf.cpp \
    main.cpp \
    FmMain.cpp

HEADERS += \
    ../Libs/SelfMade/u_Qstrings.h \
    FileWrap.h \
    FmMain.h \
    Ttf.h

INCLUDEPATH += \
    ../Libs/SelfMade

FORMS += \
    FmMain.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
