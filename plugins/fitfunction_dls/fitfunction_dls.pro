TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_dls
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/gsl.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsdls.h \
           qffitfunctiondlsg2.h \
    qffitfunctiondlsg2ind.h \
    qffitfunctiondlsg2indext.h \
    qffitfunctiondlsg2_gaussiantaudist.h

SOURCES += qfpfitfunctionsdls.cpp\
           qffitfunctiondlsg2.cpp \
    qffitfunctiondlsg2ind.cpp \
    qffitfunctiondlsg2indext.cpp \
    qffitfunctiondlsg2_gaussiantaudist.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_dls.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE

