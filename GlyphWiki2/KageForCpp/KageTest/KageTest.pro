TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../Lib/2d.cpp \
        ../Lib/2dstroke.cpp \
        ../Lib/drawing.cpp \
        ../Lib/polygon.cpp \
        main.cpp

INCLUDEPATH += \
        ../Lib

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    QMAKE_CXXFLAGS_RELEASE += -O3
    LIBS += -static -lpthread
}

HEADERS += \
    ../Lib/2d.h \
    ../Lib/2dstroke.h \
    ../Lib/defs.h \
    ../Lib/drawing.h \
    ../Lib/polygon.h \
    ../Lib/vec.h
