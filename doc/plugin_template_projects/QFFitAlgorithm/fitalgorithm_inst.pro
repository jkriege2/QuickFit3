TEMPLATE = lib
CONFIG += plugin

TARGET = target_id
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/


# Input
HEADERS += fitalgorithm_inst.h \
           fitalgorithm_inst_a1config.h \
		   fitalgorithm_inst_a1.h 

SOURCES += fitalgorithm_inst.cpp \
           fitalgorithm_inst_a1config.cpp \
		   fitalgorithm_inst_a1.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.target_id.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += gui xml svg
CONFIG += exceptions rtti stl
