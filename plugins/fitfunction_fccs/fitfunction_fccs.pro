TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_fccs
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsfccs.h \
           qffitfunctionfccsnormaldiff3d.h \
    qffitfunctionsfccsfwdiff2colorccf.h \
    qffitfunctionsfccsfwdiff2coloracfr.h \
    qffitfunctionsfccsfwdiff2coloracfg.h \
    qffitfunctionsfccsfwadiff2colorccf.h \
    qffitfunctionsfccsfwadiff2coloracfr.h \
    qffitfunctionsfccsfwadiff2coloracfg.h \
    fcstools.h \
    qffitfunctionsfccsfwdiff2colortcccf.h \
    qffitfunctionsfccsfwdiff2colortcacfr.h \
    qffitfunctionsfccsfwdiff2colortcacfg.h \
    ../interfaces/qffitfunctionconfigforglobalfitinterface.h \
    qffitfunctionsfccsfwdiff2colortcsepccf.h \
    qffitfunctionsfccsfwdiff2colortcsepacfr.h \
    qffitfunctionsfccsfwdiff2colortcsepacfg.h \
    qffitfunctionsfccsfwdiff2colorsepccf.h \
    qffitfunctionsfccsfwdiff2colorsepacfr.h \
    qffitfunctionsfccsfwdiff2colorsepacfg.h

SOURCES += qfpfitfunctionsfccs.cpp\
           qffitfunctionfccsnormaldiff3d.cpp \
    qffitfunctionsfccsfwdiff2colorccf.cpp \
    qffitfunctionsfccsfwdiff2coloracfr.cpp \
    qffitfunctionsfccsfwdiff2coloracfg.cpp \
    qffitfunctionsfccsfwadiff2colorccf.cpp \
    qffitfunctionsfccsfwadiff2coloracfr.cpp \
    qffitfunctionsfccsfwadiff2coloracfg.cpp \
    fcstools.cpp \
    qffitfunctionsfccsfwdiff2colortcccf.cpp \
    qffitfunctionsfccsfwdiff2colortcacfr.cpp \
    qffitfunctionsfccsfwdiff2colortcacfg.cpp \
    qffitfunctionsfccsfwdiff2colortcsepccf.cpp \
    qffitfunctionsfccsfwdiff2colortcsepacfr.cpp \
    qffitfunctionsfccsfwdiff2colortcsepacfg.cpp \
    qffitfunctionsfccsfwdiff2colorsepccf.cpp \
    qffitfunctionsfccsfwdiff2colorsepacfr.cpp \
    qffitfunctionsfccsfwdiff2colorsepacfg.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_fccs.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS

