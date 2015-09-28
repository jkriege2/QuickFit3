#-------------------------------------------------
#
# Project created by QtCreator 2012-05-08T19:57:46
#
#-------------------------------------------------

QT       += core gui

TARGET = jkserial_testQt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../QFSerialConnection.cpp \
    ../../cpptools.cpp

HEADERS  += mainwindow.h \
    ../../QFSerialConnection.h \
    ../../cpptools.h

FORMS    += mainwindow.ui
