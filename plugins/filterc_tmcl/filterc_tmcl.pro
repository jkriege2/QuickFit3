TEMPLATE = lib
CONFIG += plugin

TARGET = filterc_tmcl
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfextensiontmclfilterchanger.h \
    ../interfaces/qfextensionfilterchanger.h \
    ../base_classes/qf3comportmanager.h \
    ../../../../../LIB/trunk/jkserialconnection.h \
    ../base_classes/qf3tmclprotocolhandler.h \
    ../interfaces/qfextensionglobalsettingsreadwrite.h

SOURCES += qfextensiontmclfilterchanger.cpp \
    ../base_classes/qf3comportmanager.cpp \
    ../../../../../LIB/trunk/jkserialconnection.cpp \
    ../base_classes/qf3tmclprotocolhandler.cpp

FORMS =

RESOURCES += qfextensiontmclfilterchanger.qrc

TRANSLATIONS= ./translations/de.filterc_tmcl.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



