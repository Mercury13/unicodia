QT       += core gui widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32-g++ {
    # To simplify debugging, we statically link these libraries
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
    # Qt — system headers
    QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]
}

SOURCES += \
    ../Libs/PugiXml/pugixml.cpp \
    ../Libs/SelfMade/Strings/u_Strings.cpp \
    WiViewer.cpp \
    main.cpp \
    FmMain.cpp

HEADERS += \
    ../Libs/PugiXml/pugiconfig.hpp \
    ../Libs/PugiXml/pugixml.hpp \
    ../Libs/SelfMade/Strings/u_Strings.h \
    ../Libs/SelfMade/u_Vector.h \
    FmMain.h \
    WiViewer.h

INCLUDEPATH += \
    ../Libs/PugiXml \
    ../Libs/SelfMade \
    ../Libs/SelfMade/Strings

FORMS += \
    FmMain.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
