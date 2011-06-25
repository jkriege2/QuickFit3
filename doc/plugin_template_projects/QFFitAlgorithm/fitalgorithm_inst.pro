TEMPLATE = lib
CONFIG += plugin

TARGET = target_id
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/

LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += fitalgorithm_inst.h \
           fitalgorithm_instconfig.h

SOURCES += fitalgorithm_inst.cpp \
           fitalgorithm_instconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.target_id.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += gui xml svg
CONFIG += exceptions rtti stl
