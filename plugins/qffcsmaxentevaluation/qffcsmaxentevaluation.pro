TEMPLATE = lib
CONFIG += plugin

TARGET = fcs_maxent
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qffcsmaxentevaluation.h \
           qffcsmaxentevaluation_item.h \
           qffcsmaxentevaluation_editor.h

SOURCES += qffcsmaxentevaluation.cpp \
           qffcsmaxentevaluation_item.cpp \
           qffcsmaxentevaluation_editor.cpp

FORMS = qffcsmaxentevaluation_editor.ui   

RESOURCES += qffcsmaxentevaluation.qrc

TRANSLATIONS= ./translations/de.fcs_maxent.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl



