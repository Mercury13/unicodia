TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}

SOURCES += \
        ../Libs/PugiXml/pugixml.cpp \
        ../Libs/SelfMade/Strings/u_Strings.cpp \
        main.cpp

HEADERS += \
    ../Libs/PugiXml/pugixml.hpp \
    ../Libs/SelfMade/i_ByteSwap.h

INCLUDEPATH += \
    ../Libs/PugiXml \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Strings
