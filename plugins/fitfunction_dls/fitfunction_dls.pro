TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunctions_dls
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsdls.h \
           qffitfunctiondlsg2.h \
    qffitfunctiondlsg2ind.h

SOURCES += qfpfitfunctionsdls.cpp\
           qffitfunctiondlsg2.cpp \
    qffitfunctiondlsg2ind.cpp

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
                        $$TRANSLATIONS


