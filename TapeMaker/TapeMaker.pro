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
        main.cpp \
        optstorage.cpp

HEADERS += \
    ../Libs/PugiXml/pugixml.hpp \
    ../Libs/SelfMade/i_ByteSwap.h \
    optstorage.h

INCLUDEPATH += \
    ../Libs/PugiXml \
    ../Libs/Sha256 \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Strings
