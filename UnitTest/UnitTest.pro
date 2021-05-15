TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

win32-g++: {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    QMAKE_LFLAGS += -fuse-ld=lld
    LIBS += -static -lpthread
}

SOURCES += \
    ../AutoBuilder/data.cpp \
    ../Libs/GoogleTest/src/gtest-all.cc \
    ../Libs/GoogleTest/src/gtest_main.cc \
    ../Libs/SelfMade/u_Strings.cpp \
    test_Decapitalize.cpp

HEADERS += \
    ../AutoBuilder/data.h \
    ../Libs/SelfMade/u_Strings.h

INCLUDEPATH += \
    ../AutoBuilder \
    ../Libs/GoogleTest \
    ../Libs/GoogleTest/include \
    ../Libs/SelfMade
