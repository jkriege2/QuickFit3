TEMPLATE = lib
CONFIG += plugin

TARGET = qffcsfitfuncs
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
#DLLDESTDIR = $$DESTDIR

LIBS += -L$$QFOUTPUT -lquickfit3lib


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
