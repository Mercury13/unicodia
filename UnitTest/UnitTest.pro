TEMPLATE = app
CONFIG += console c++2a
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
    ../Libs/SelfMade/u_SearchEngine.cpp \
    ../Libs/SelfMade/Strings/u_Strings.cpp \
    ../Unicodia/Wiki.cpp \
    test_Decapitalize.cpp \
    test_DumbSp.cpp \
    test_Iterator.cpp \
    test_Search.cpp \
    test_Strings.cpp \
    test_Wiki.cpp

HEADERS += \
    ../AutoBuilder/data.h \
    ../Libs/SelfMade/u_Iterator.h \
    ../Libs/SelfMade/u_SearchEngine.h \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Unicodia/Wiki.h

INCLUDEPATH += \
    ../AutoBuilder \
    ../Libs/GoogleTest \
    ../Libs/GoogleTest/include \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Strings \
    ../Unicodia
