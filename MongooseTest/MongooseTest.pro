TEMPLATE = app
CONFIG += console c++2a
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lws2_32 -lcrypto -lssl -lcrypt32

SOURCES += \
        ../Libs/Mongoose/mongoose.c \
        main.cpp

HEADERS += \
    ../Libs/Mongoose/mongoose.h

INCLUDEPATH += \
    ../Libs/Mongoose

