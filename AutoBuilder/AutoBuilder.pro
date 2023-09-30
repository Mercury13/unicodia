TEMPLATE = app
CONFIG += console c++2a
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
        ../Libs/SelfMade/Strings/u_Strings.cpp \
        data.cpp \
        egyptian.cpp \
        entities.cpp \
        forget.cpp \
        library.cpp \
        loader.cpp \
        main.cpp \
        sutton.cpp \
        textbase.cpp \
        utils.cpp

HEADERS += \
    ../Libs/PugiXml/pugiconfig.hpp \
    ../Libs/PugiXml/pugixml.hpp \
    ../Libs/SelfMade/Mojibake/mojibake.h \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Libs/SelfMade/u_Array.h \
    ../Libs/SelfMade/u_TypedFlags.h \
    ../Libs/SelfMade/u_Vector.h \
    ../Unicodia/Uc/UcCp.h \
    ../Unicodia/Uc/UcFlags.h \
    data.h \
    egyptian.h \
    entities.h \
    forget.h \
    library.h \
    loader.h \
    sutton.h \
    textbase.h \
    utils.h


INCLUDEPATH += \
    ../Libs/PugiXml \
    ../Libs/RapidJson \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Mojibake \
    ../Libs/SelfMade/Strings \
    ../Libs/Zippy \
    ../Unicodia/Uc
