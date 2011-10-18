TEMPLATE = lib
CONFIG += plugin

TARGET = qffcsfitfuncs
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
#DLLDESTDIR = $$DESTDIR



# Input
HEADERS += fcs_fitfuctions.h \
           qffitfunctionfcsmultidiffusion.h \
           qffitfunctionfcsadiff.h \
           qffitfunctionfcsdiff.h \
           qffitfunctionfcssimplediff.h

SOURCES += fcs_fitfuctions.cpp\
           qffitfunctionfcsmultidiffusion.cpp \
           qffitfunctionfcsadiff.cpp \
           qffitfunctionfcsdiff.cpp \
           qffitfunctionfcssimplediff.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS += ./translations/de.qffcsfitfuncs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl


SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS
