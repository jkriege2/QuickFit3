
include(../../extlibs/cairo.pri)


QT       += core gui

TARGET = cairoQPaintDevice_test
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += cairoQPaintDevice_test.cpp \
    qcairopaintdevice.cpp \
    qcairopaintengine.cpp

HEADERS += \
    qcairopaintdevice.h \ 
    qcairopaintengine.h
    
