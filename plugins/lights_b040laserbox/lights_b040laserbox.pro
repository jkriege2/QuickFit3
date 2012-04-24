TEMPLATE = lib
CONFIG += plugin

TARGET = lights_b040laserbox
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += \
    ../base_classes/qf3simpleb040serialprotocolhandler.h \
    ../base_classes/qf3comportmanager.h \
    ../interfaces/qfextensionlightsource.h \
    ../../../../../LIB/trunk/jkserialconnection.h \
    qfextensionb040laserbox.h

SOURCES += \
    ../base_classes/qf3simpleb040serialprotocolhandler.cpp \
    ../base_classes/qf3comportmanager.cpp \
    ../../../../../LIB/trunk/jkserialconnection.cpp \
    qfextensionb040laserbox.cpp

FORMS =

RESOURCES += qfextensionb040laserbox.qrc

TRANSLATIONS= ./translations/de.lights_b040laserbox.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



