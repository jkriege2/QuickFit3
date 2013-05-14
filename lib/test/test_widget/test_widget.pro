#-------------------------------------------------
#
# Project created by QtCreator 2013-05-14T20:25:11
#
#-------------------------------------------------

QT       += core gui

TARGET = test_widget
TEMPLATE = app

INCLUDEPATH += ../../

DEFINES += QFLIB_TEST


SOURCES += main.cpp\
        mainwindow.cpp \
    ../../qfdoubleedit.cpp

HEADERS  += mainwindow.h \
    ../../qfdoubleedit.h \
    ../../lib_imexport.h

FORMS    += mainwindow.ui
