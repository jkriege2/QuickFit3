TEMPLATE = lib
CONFIG += plugin

TARGET = cam_server
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/


# Input
HEADERS += qfecamserver.h \
           ../interfaces/qfextensioncamera.h

SOURCES += qfecamserver.cpp

FORMS =

RESOURCES += qfecamserver.qrc

TRANSLATIONS= ./translations/de.cam_server.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg network
CONFIG += exceptions rtti stl



