TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}

# This is utility, range check on release is OK
DEFINES += AT_RANGE_CHECK

SOURCES += \
        ../Libs/PugiXml/pugixml.cpp \
        ../Libs/SelfMade/u_Strings.cpp \
        data.cpp \
        main.cpp

HEADERS += \
    ../Libs/PugiXml/pugiconfig.hpp \
    ../Libs/PugiXml/pugixml.hpp \
    ../Libs/SelfMade/u_Array.h \
    ../Libs/SelfMade/u_Strings.h \
    ../Libs/SelfMade/u_TypedFlags.h \
    ../Libs/SelfMade/u_Vector.h \
    data.h


INCLUDEPATH += \
    ../Libs/PugiXml \
    ../Libs/SelfMade
