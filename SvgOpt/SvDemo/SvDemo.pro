TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../../Libs/PugiXml/pugixml.cpp \
        ../../Libs/SelfMade/Strings/u_Strings.cpp \
        ../Lib/Xcolor.cpp \
        ../Lib/Xsvg.cpp \
        main.cpp

HEADERS += \
    ../../Libs/PugiXml/pugixml.hpp \
    ../../Libs/SelfMade/Strings/u_Strings.h \
    ../Lib/Xcolor.h \
    ../Lib/Xsvg.h

INCLUDEPATH += \
    ../../Libs/PugiXml \
    ../../Libs/SelfMade/Strings \
    ../Lib

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}
