TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_tirfcs
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionstirfcs.h \
    ../interfaces/qffitfunctionconfigforglobalfitinterface.h \
    imfcstools.h \
    qffitfunctionstirfcsdiffe2.h \
    qffitfunctionstirfcsadiffe2.h \
    qffitfunctionstirfccsdiffflowe2.h


SOURCES += qfpfitfunctionstirfcs.cpp\
    imfcstools.cpp \
    qffitfunctionstirfcsdiffe2.cpp \
    qffitfunctionstirfcsadiffe2.cpp \
    qffitfunctionstirfccsdiffflowe2.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_tirfcs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE

