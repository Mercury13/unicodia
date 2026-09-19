QT += widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32-g++ {
    QMAKE_CXXFLAGS_DEBUG += -static-libgcc -static-libstdc++
    LIBS += -static -lpthread
}

SOURCES += \
    ../../Libs/PugiXml/pugixml.cpp \
    ../../Libs/SelfMade/Qt/i_OpenSave.cpp \
    ../../Libs/SelfMade/Qt/u_OpenSaveStrings.cpp \
    ../../Libs/SelfMade/Strings/u_Strings.cpp \
    main.cpp \
    FmMain.cpp \
    work.cpp

HEADERS += \
    ../../Libs/PugiXml/pugixml.hpp \
    ../../Libs/SelfMade/Qt/i_OpenSave.h \
    ../../Libs/SelfMade/Qt/u_OpenSaveStrings.h \
    ../../Libs/SelfMade/Strings/u_Strings.h \
    FmMain.h \
    work.h

INCLUDEPATH += \
    ../../Libs/PugiXml \
    ../../Libs/SelfMade \
    ../../Libs/SelfMade/Qt \
    ../../Libs/SelfMade/Strings

FORMS += \
    FmMain.ui

win32 {
    LIBS += -lcomdlg32
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
