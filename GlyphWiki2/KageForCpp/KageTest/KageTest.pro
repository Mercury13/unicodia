TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../../../Libs/SelfMade/Strings/u_Strings.cpp \
        ../Lib/2d.cpp \
        ../Lib/2dstroke.cpp \
        ../Lib/base.cpp \
        ../Lib/drawing.cpp \
        ../Lib/polygon.cpp \
        ../Lib/util.cpp \
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
    ../Lib/2d.h \
    ../Lib/2dstroke.h \
    ../Lib/base.h \
    ../Lib/defs.h \
    ../Lib/drawing.h \
    ../Lib/polygon.h \
    ../Lib/style.h \
    ../Lib/util.h \
    ../Lib/list.h

INCLUDEPATH += \
    ../../../Libs/SelfMade/Strings \
    ../../../Libs/SelfMade \
    ../Lib
