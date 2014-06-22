
include(../../extlibs/cairo.pri)


QT       += core gui

TARGET = cairo4qt_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += cairo4qt_test.cpp \
    cairo4qt/src/brushpattern.cpp \
    cairo4qt/src/qcairopaintdevice.cpp \
    cairo4qt/src/qpaintengine_cairo.cpp

HEADERS += \
    cairo4qt/src/qcairopaintdevice.h \
    cairo4qt/src/qpaintengine_cairo_p.h \
    cairo4qt/src/qpaintengine_p.h \
    cairo4qt/src/qt_x11_p.h
