TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

win32-g++: {
    QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}

SOURCES += \
    ../AutoBuilder/decap/decap.cpp \
    ../AutoBuilder/decap/dic.cpp \
    ../AutoBuilder/decap/exceptions.cpp \
    ../AutoBuilder/forget.cpp \
    ../Libs/GoogleTest/src/gtest-all.cc \
    ../Libs/GoogleTest/src/gtest_main.cc \
    ../Libs/L10n/LocFmt.cpp \
    ../Libs/PugiXml/pugixml.cpp \
    ../Libs/SelfMade/Strings/u_Strings.cpp \
    ../Libs/SelfMade/u_Version.cpp \
    ../SvgOpt/Lib/Xcolor.cpp \
    ../SvgOpt/Lib/Xdic.cpp \
    ../SvgOpt/Lib/Xstyle.cpp \
    ../SvgOpt/Lib/Xsvg.cpp \
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
    test_Wiki.cpp \
    test_Xcolor.cpp \
    test_Xdic.cpp \
    test_Xstyle.cpp \
    text_Xsvg.cpp

HEADERS += \
    ../AutoBuilder/data.h \
    ../AutoBuilder/forget.h \
    ../Libs/L10n/LocFmt.h \
    ../Libs/PugiXml/pugixml.hpp \
    ../Libs/SelfMade/u_Iterator.h \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Libs/SelfMade/u_Version.h \
    ../SvgOpt/Lib/Xcolor.h \
    ../SvgOpt/Lib/Xdic.h \
    ../SvgOpt/Lib/Xid.h \
    ../SvgOpt/Lib/Xstyle.h \
    ../SvgOpt/Lib/Xsvg.h \
    ../Unicodia/Search/engine.h \
    ../Unicodia/Search/trie.h \
    ../Unicodia/Wiki.h

INCLUDEPATH += \
    ../AutoBuilder \
    ../Libs/GoogleTest \
    ../Libs/GoogleTest/include \
    ../Libs/L10n \
    ../Libs/PugiXml \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Strings \
    ../SvgOpt/Lib \
    ../Unicodia \
    ../Unicodia/Uc
