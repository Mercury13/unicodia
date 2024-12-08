TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../../../Libs/SelfMade/Strings/u_Strings.cpp \
        main.cpp

win32-g++ {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}

HEADERS += \
    ../../../Libs/SelfMade/Strings/u_Strings.h

INCLUDEPATH += \
    ../../../Libs/SelfMade \
    ../../../Libs/SelfMade/Strings \
