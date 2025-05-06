TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../../../Libs/SelfMade/Strings/u_Strings.cpp \
        ../kage/2d.cpp \
        ../kage/2dstroke.cpp \
        ../kage/base.cpp \
        ../kage/drawing.cpp \
        ../kage/mincho.cpp \
        ../kage/polygon.cpp \
        ../kage/util.cpp \
        main.cpp

INCLUDEPATH += \
        ../Lib

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    QMAKE_CXXFLAGS_RELEASE += -O3
    LIBS += -static -lpthread
}

HEADERS += \
    ../../../Libs/SelfMade/Strings/u_Strings.h \
    ../kage/2d.h \
    ../kage/2dstroke.h \
    ../kage/base.h \
    ../kage/defs.h \
    ../kage/drawing.h \
    ../kage/mincho.h \
    ../kage/polygon.h \
    ../kage/style.h \
    ../kage/util.h \
    ../kage/list.h

INCLUDEPATH += \
    ../../../Libs/SelfMade/Strings \
    ../../../Libs/SelfMade \
    ..
