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
        ../Libs/SelfMade/Geo2d/g2svgUtils.cpp \
        FixupList.cpp \
        main.cpp

HEADERS += \
    ../Libs/PugiXml/pugiconfig.hpp \
    ../Libs/PugiXml/pugixml.hpp \
    ../Libs/SelfMade/Geo2d/g2defines.h \
    ../Libs/SelfMade/Geo2d/g2svgUtils.h \
    FixupList.h

INCLUDEPATH += \
    ../Libs/PugiXml \
    ../Libs/SelfMade/Geo2d
