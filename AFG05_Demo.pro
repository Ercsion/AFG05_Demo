#-------------------------------------------------
#
# Project created by QtCreator 2016-05-13T22:00:34
#
#-------------------------------------------------


QT         += core gui
DESTDIR    += ./build
TARGET      = AFG05_Demo

CONFIG      += qt warn_on release
UI_DIR       = inc
MOC_DIR      = inc
RCC_DIR      = inc
OBJECTS_DIR  = obj

DEPENDPATH   += . ui inc res
INCLUDEPATH  += . ui
TEMPLATE      = app


SOURCES += main.cpp\
    MainWindow.cpp \
    qextserialport.cpp \
    FaceDetectData.cpp

win32 { SOURCES += qextserialport_win.cpp }
unix { SOURCES += qextserialport_unix.cpp }

HEADERS  += MainWindow.h \
    qextserialport_global.h \
    qextserialport.h \
    myhelper.h \
    FaceDetectData.h

FORMS    += MainWindow.ui
