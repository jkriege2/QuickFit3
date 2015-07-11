include(xits.pri)
include(asana.pri)

LIBS +=


SOURCES = jkqtmathtext_test.cpp \
          ../../jkqtmathtext.cpp \
          ../../jkqttools.cpp \
          ../../../tools.cpp \
          ../../../highrestimer.cpp \
    testform.cpp

RCC_DIR = rccs

CONFIG += qt windows


TARGET = jkqtplot_test

QT += core gui svg

UI_DIR = .uics

HEADERS = ../../jkqttools.h \
          ../../jkqtmathtext.h \
          ../../../tools.h \
          ../../../highrestimer.h \
    testform.h

OBJECTS_DIR = .objs

INCLUDEPATH +=

MOC_DIR = .mocs

FORMS = \
    testform.ui

DESTDIR = ./

DEFINES += AUTOLOAD_XITS_FONTS AUTOLOAD_Asana_FONTS

