TEMPLATE = lib
CONFIG += plugin

TARGET = spim_lightsheet_eval
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
include(../../libquickfitwidgets.pri)


# Input
HEADERS += qfspimlightsheetevaluation.h \
           qfspimlightsheetevaluation_item.h \
           qfspimlightsheetevaluation_editor.h \
    ../interfaces/qfrdrimagestackinterface.h \
    ../../../../../LIB/trunk/statistics_tools.h

SOURCES += qfspimlightsheetevaluation.cpp \
           qfspimlightsheetevaluation_item.cpp \
           qfspimlightsheetevaluation_editor.cpp \
    ../../../../../LIB/trunk/statistics_tools.cpp

FORMS = qfspimlightsheetevaluation_editor.ui   

RESOURCES += qfspimlightsheetevaluation.qrc

TRANSLATIONS= ./translations/de.spim_lightsheet_eval.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl


ASSETS_TESTDATA.files = ./assets/test_data/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/test_data/
INSTALLS += ASSETS_TESTDATA


SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS

