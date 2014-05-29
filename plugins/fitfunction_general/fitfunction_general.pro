TEMPLATE = lib
CONFIG += plugin

TARGET = fitfunction_general
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfgeneralfitfunctionsplugin.h \
           qffitfunctiongeneralsigmoid.h \
    ../../../../../LIB/trunk/statistics_tools.h \
    qffitfunctiongeneralgaussianvar.h \
    qffitfunctiongenerallorentzian.h \
    qffitfunctiongeneralpowerlaw.h \
    qffitfunctiongeneralexp.h \
    qffitfunctiongeneraldblexp.h \
    qffitfunctiongeneralpolynom.h \
    qffitfunctiongeneralsine.h \
    qffitfunctiongenerallognormal.h \
    qffitfunctiongeneral2lognormal.h \
    qffitfunctiongeneralline.h \
    qffitfunctiongeneral3gaussianvar.h \
    qffitfunctiongeneral2gaussianvar.h \
    qffitfunctiongeneralmodhill5p.h \
    qffitfunctiongeneralhill.h \
    qffitfunctiongeneralcosine.h \
    qffitfunctiongeneralpolynomcentered.h \
    qffitfunctiongeneralgaussiandistvar.h

SOURCES += qfgeneralfitfunctionsplugin.cpp\
           qffitfunctiongeneralsigmoid.cpp \
    ../../../../../LIB/trunk/statistics_tools.cpp \
    qffitfunctiongeneralgaussianvar.cpp \
    qffitfunctiongenerallorentzian.cpp \
    qffitfunctiongeneralpowerlaw.cpp \
    qffitfunctiongeneralexp.cpp \
    qffitfunctiongeneraldblexp.cpp \
    qffitfunctiongeneralsine.cpp \
    qffitfunctiongeneralpolynom.cpp \
    qffitfunctiongenerallognormal.cpp \
    qffitfunctiongeneral2lognormal.cpp \
    qffitfunctiongeneralline.cpp \
    qffitfunctiongeneral3gaussianvar.cpp \
    qffitfunctiongeneral2gaussianvar.cpp \
    qffitfunctiongeneralmodhill5p.cpp \
    qffitfunctiongeneralhill.cpp \
    qffitfunctiongeneralcosine.cpp \
    qffitfunctiongeneralpolynomcentered.cpp \
    qffitfunctiongeneralgaussiandistvar.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunction_general.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE

