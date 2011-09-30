TEMPLATE = lib
CONFIG += plugin

TARGET = cam_radhard2
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/usb.pri)

DESTDIR = $$QFOUTPUT/plugins


DEFINES += TARGETNAME=$$TARGET

include(../../libquickfitwidgets.pri)

# Input
HEADERS += cam_radhard2.h \
           radhard2lib/radhard2.h \
           radhard2lib/radhard2lib.h \
           ../base_classes/radhard2flash.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h \
    qfradhard2flashtool.h

SOURCES += cam_radhard2.cpp \
           radhard2lib/radhard2.cpp \
           ../base_classes/radhard2flash.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp \
    qfradhard2flashtool.cpp

FORMS = \
    qfradhard2flashtool.ui

RESOURCES += cam_radhard2.qrc

TRANSLATIONS= ./translations/de.cam_radhard2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += gui xml svg
CONFIG += exceptions rtti stl



