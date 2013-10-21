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
    qffitfunctionstirfccsdiffflowe2.h \
    qffitfunctionstirffccsfw2csep2ddiffxy2colorccf.h \
    qffitfunctionstirffccsfw2csep2ddiffxy2coloracfr.h \
    qffitfunctionstirffccsfw2csep2ddiffxy2coloracfg.h \
    qffitfunctionstirffccsfw2ddiffxy2colorccf.h \
    qffitfunctionstirffccsfw2ddiffxy2coloracfr.h \
    qffitfunctionstirffccsfw2ddiffxy2coloracfg.h \
    qffitfunctionstirffccsfw2ddiffxy2colorccf.h


SOURCES += qfpfitfunctionstirfcs.cpp\
    imfcstools.cpp \
    qffitfunctionstirfcsdiffe2.cpp \
    qffitfunctionstirfcsadiffe2.cpp \
    qffitfunctionstirfccsdiffflowe2.cpp \
    qffitfunctionstirffccsfw2csep2ddiffxy2colorccf.cpp \
    qffitfunctionstirffccsfw2csep2ddiffxy2coloracfr.cpp \
    qffitfunctionstirffccsfw2csep2ddiffxy2coloracfg.cpp \
    qffitfunctionstirffccsfw2ddiffxy2colorccf.cpp \
    qffitfunctionstirffccsfw2ddiffxy2coloracfr.cpp \
    qffitfunctionstirffccsfw2ddiffxy2coloracfg.cpp

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

