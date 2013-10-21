TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_gslmathparserextensions
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../extlibs/gsl.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfe_gslmathparserextensions.h \
    qfe_gslmathparserextensionsfunctions.h \
    ../../../../../LIB/trunk/statistics_tools.h

SOURCES += qfe_gslmathparserextensions.cpp \
    qfe_gslmathparserextensionsfunctions.cpp \
    ../../../../../LIB/trunk/statistics_tools.cpp

FORMS =

RESOURCES += qfe_gslmathparserextensions.qrc

TRANSLATIONS= ./translations/de.qfe_gslmathparserextensions.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

HELPREF_FILES.files = ./help/parserreference/*.*
HELPREF_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/

HELPREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPREFPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/pic/

INSTALLS +=  HELPREF_FILES HELPREFPIC_FILES


QT += gui xml svg
CONFIG += exceptions rtti stl



SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE
