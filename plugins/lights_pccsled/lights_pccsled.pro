TEMPLATE = lib
CONFIG += plugin

TARGET = lights_pccsled
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
    qfextensionpccsled.h \
    ../interfaces/qfextensionshutter.h

SOURCES += \
    ../base_classes/qf3simpleb040serialprotocolhandler.cpp \
    ../base_classes/qf3comportmanager.cpp \
    ../../../../../LIB/trunk/jkserialconnection.cpp \
    qfextensionpccsled.cpp

FORMS =

RESOURCES += qfextensionpccpled.qrc

TRANSLATIONS= ./translations/de.lights_pccsled.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl
