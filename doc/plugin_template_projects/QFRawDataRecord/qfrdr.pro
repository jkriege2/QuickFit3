TEMPLATE = lib
CONFIG += plugin


TARGET = target_id
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfrdr.h \
           qfrdr_data.h \
		   qfrdr_dataeditor.h

SOURCES += qfrdr.cpp \
           qfrdr_data.cpp \
		   qfrdr_dataeditor.cpp

FORMS = 

RESOURCES += qfrdr.qrc

TRANSLATIONS= ./translations/de.target_id.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl



