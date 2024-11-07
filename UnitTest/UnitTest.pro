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
    ../AutoBuilder/decap/decap.cpp \
    ../AutoBuilder/decap/dic.cpp \
    ../AutoBuilder/forget.cpp \
    ../Libs/GoogleTest/src/gtest-all.cc \
    ../Libs/GoogleTest/src/gtest_main.cc \
    ../Libs/L10n/LocFmt.cpp \
    ../Libs/SelfMade/Strings/u_Strings.cpp \
    ../Libs/SelfMade/u_Version.cpp \
    ../Unicodia/Search/engine.cpp \
    ../Unicodia/Wiki.cpp \
    test_Decapitalize.cpp \
    test_DumbSp.cpp \
    test_Fmt.cpp \
    test_Forget.cpp \
    test_Iterator.cpp \
    test_Search.cpp \
    test_Strings.cpp \
    test_Trie.cpp \
    test_Version.cpp \
    test_Wiki.cpp

HEADERS += \
    ../AutoBuilder/data.h \
    ../AutoBuilder/forget.h \
    ../Libs/L10n/LocFmt.h \
    ../Libs/SelfMade/u_Iterator.h \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Libs/SelfMade/u_Version.h \
    ../Unicodia/Search/engine.h \
    ../Unicodia/Search/trie.h \
    ../Unicodia/Wiki.h

INCLUDEPATH += \
    ../AutoBuilder \
    ../Libs/GoogleTest \
    ../Libs/GoogleTest/include \
    ../Libs/L10n \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Strings \
    ../Unicodia \
    ../Unicodia/Uc
