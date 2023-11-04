TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}

SOURCES += \
    ../../Libs/PugiXml/pugixml.cpp \
    ../../Libs/SelfMade/Noto/xml.cpp \
    ../../Libs/SelfMade/Strings/u_Strings.cpp \
    main.cpp

HEADERS += \
    ../../Libs/PugiXml/pugixml.hpp \
    ../../Libs/SelfMade/Noto/xml.h \
    ../../Libs/SelfMade/Strings/u_Strings.h \
    ../../Libs/SelfMade/u_Vector.h


INCLUDEPATH += \
    ../../Libs/PugiXml \
    ../../Libs/SelfMade \
    ../../Libs/SelfMade/Strings
