QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a c++20

CONFIG(debug, debug|release) {
    DEFINES += AT_RANGE_CHECK
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../Libs/SelfMade/Strings/u_Strings.cpp \
    ../KageLibs/kage.cpp \
    ../KageLibs/kageQt.cpp \
    main.cpp \
    FmMain.cpp

HEADERS += \
    ../../Libs/SelfMade/Strings/u_Strings.h \
    ../KageLibs/kage.h \
    ../KageLibs/kageQt.h \
    ../KageLibs/u_MaybeInt.h \
    FmMain.h

FORMS += \
    FmMain.ui

INCLUDEPATH += \
    ../../Libs/SelfMade  \
    ../../Libs/SelfMade/Strings  \
    ../KageLibs

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32-g++ {
    # To simplify debugging, we statically link these libraries
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
    # Qt — system headers
    QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]
    CONFIG(debug, debug|release) {
        LIBS += -static -lpthread
    }
}
