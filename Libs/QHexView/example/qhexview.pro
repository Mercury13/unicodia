TEMPLATE = app
TARGET = qhexview
INCLUDEPATH += . ../include

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Input
HEADERS = MainWindow.h              \
          ../include/QHexView.h

SOURCES = MainWindow.cpp            \
          main.cpp                  \
          ../src/QHexView.cpp 
