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
    qffitfunctionsfccsfwdiff2colortcacfg.h

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
    qffitfunctionsfccsfwdiff2colortcacfg.cpp

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

