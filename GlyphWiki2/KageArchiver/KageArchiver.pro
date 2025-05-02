TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../../Libs/SelfMade/Strings/u_Strings.cpp \
        main.cpp

HEADERS += \
    ../../Libs/SelfMade/Strings/u_Strings.h \
    ../../Libs/SelfMade/u_Vector.h

INCLUDEPATH += \
    ../../Libs/SelfMade \
    ../../Libs/SelfMade/Strings

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    QMAKE_CXXFLAGS_RELEASE += -O3
    LIBS += -static -lpthread
}
