TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ../../Libs/SelfMade/Fonts/MemFont.cpp \
    ../../Libs/SelfMade/i_MemStream.cpp \
    main.cpp

HEADERS += \
    ../../Libs/SelfMade/Fonts/MemFont.h \
    ../../Libs/SelfMade/Fonts/MemFontDefs.h \
    ../../Libs/SelfMade/i_MemStream.h

INCLUDEPATH += \
    ../../Libs/ \
    ../../Libs/SelfMade

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}
