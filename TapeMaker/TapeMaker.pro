TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}

SOURCES += \
        main.cpp

HEADERS += \
    ../Libs/SelfMade/i_ByteSwap.h

INCLUDEPATH += \
    ../Libs/SelfMade
