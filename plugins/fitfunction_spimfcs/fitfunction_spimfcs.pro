TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_spimfcs
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsspimfcs.h \
           qffitfunctionsspimfcsdiff.h \
    qffitfunctionsspimfcsadiff.h \
    qffitfunctionsspimfcsdiffe2.h \
    qffitfunctionsspimfccsdiffflow.h \
    qffitfunctionsspimfccsdiff2color.h \
    qffitfunctionsspimfcsdiffc.h \
    qffitfunctionsspimfcsdiffe2_newveff.h \
    qffitfunctionsspimfcsdiffce2.h \
    qffitfunctionsspimfcsadiffe2.h \
    imfcstools.h \
    qffitfunctionsspimfcsadiffce2.h \
    qffitfunctionsspimfccsfwdiff2colorccf.h \
    qffitfunctionsspimfccsfwdiff2coloracfr.h \
    qffitfunctionsspimfccsfwdiff2coloracfg.h \
    qffitfunctionsspimfccsfwfactordiff2colorccf.h \
    qffitfunctionsspimfccsfwfactordiff2coloracfr.h \
    qffitfunctionsspimfccsfwfactordiff2coloracfg.h \
    qffitfunctionsspimfccsfwadiff2colorccf.h \
    qffitfunctionsspimfccsfwadiff2coloracfr.h \
    qffitfunctionsspimfccsfwadiff2coloracfg.h \
    qffitfunctionsspimfccsfw2cdiff2colorccf.h \
    qffitfunctionsspimfccsfw2cdiff2coloracfr.h \
    qffitfunctionsspimfccsfw2cdiff2coloracfg.h \
    qffitfunctionsspimfccsdiff.h


SOURCES += qfpfitfunctionsspimfcs.cpp\
           qffitfunctionsspimfcsdiff.cpp \
    qffitfunctionsspimfcsadiff.cpp \
    qffitfunctionsspimfcsdiffe2.cpp \
    qffitfunctionsspimfccsdiffflow.cpp \
    qffitfunctionsspimfccsdiff2color.cpp \
    qffitfunctionsspimfcsdiffc.cpp \
    qffitfunctionsspimfcsdiffe2_newveff.cpp \
    qffitfunctionsspimfcsdiffce2.cpp \
    qffitfunctionsspimfcsadiffe2.cpp \
    imfcstools.cpp \
    qffitfunctionsspimfcsadiffce2.cpp \
    qffitfunctionsspimfccsfwdiff2coloracfg.cpp \
    qffitfunctionsspimfccsfwdiff2colorccf.cpp \
    qffitfunctionsspimfccsfwfactordiff2coloracfr.cpp \
    qffitfunctionsspimfccsfwfactordiff2coloracfg.cpp \
    qffitfunctionsspimfccsfwfactordiff2colorccf.cpp \
    qffitfunctionsspimfccsfwdiff2coloracfr.cpp \
    qffitfunctionsspimfccsfwadiff2coloracfg.cpp \
    qffitfunctionsspimfccsfwadiff2colorccf.cpp \
    qffitfunctionsspimfccsfwadiff2coloracfr.cpp \
    qffitfunctionsspimfccsfw2cdiff2colorccf.cpp \
    qffitfunctionsspimfccsfw2cdiff2coloracfr.cpp \
    qffitfunctionsspimfccsfw2cdiff2coloracfg.cpp \
    qffitfunctionsspimfccsdiff.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_spimfcs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS

