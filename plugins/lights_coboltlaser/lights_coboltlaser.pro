TEMPLATE = lib
CONFIG += plugin

TARGET = lights_coboltlaser
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += \
    ../base_classes/qf3comportmanager.h \
    ../interfaces/qfextensionlightsource.h \
    ../../../../../LIB/trunk/jkserialconnection.h \
    qfextensioncoboltlaser.h \
    qfcoboltlaserprotocolhandler.h \
    ../base_classes/qfsimpleserialprotocolhandler.h


SOURCES += \
    ../base_classes/qf3comportmanager.cpp \
    ../../../../../LIB/trunk/jkserialconnection.cpp \
    qfextensioncoboltlaser.cpp \
    qfcoboltlaserprotocolhandler.cpp \
    ../base_classes/qfsimpleserialprotocolhandler.cpp

FORMS =

RESOURCES += lights_coboltlaser.qrc

TRANSLATIONS= ./translations/de.lights_coboltlaser.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



