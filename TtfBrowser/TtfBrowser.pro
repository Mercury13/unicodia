QT       += core gui widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Libs/SelfMade/Qt/i_OpenSave.cpp \
    ../Libs/SelfMade/Qt/u_OpenSaveStrings.cpp \
    ../Libs/SelfMade/Strings/u_Strings.cpp \
    main.cpp \
    FmMain.cpp

HEADERS += \
    ../Libs/SelfMade/Qt/i_OpenSave.h \
    ../Libs/SelfMade/Qt/u_OpenSaveStrings.h \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Libs/SelfMade/u_Vector.h \
    FmMain.h

FORMS += \
    FmMain.ui

INCLUDEPATH += \
    ../Libs/SelfMade/  \
    ../Libs/SelfMade/Qt \
    ../Libs/SelfMade/Strings

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    LIBS += -lcomdlg32
}

win32-g++ {
    # To simplify debugging, we statically link these libraries
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
    # Qt — system headers
    QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]
}