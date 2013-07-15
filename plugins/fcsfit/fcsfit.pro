TEMPLATE = lib
CONFIG += plugin

TARGET = qfevalfcsfit
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS +=  qfpevalfcsfit.h \
            qffcsfitevaluation.h \
            qffcsfitevaluationeditor.h \
            ../interfaces/qfrdrfcsdatainterface.h \
            ../interfaces/qfrdrcountratesinterface.h \
            ../../../../../LIB/trunk/statistics_tools.h \
            dlgestimatefocalvolume.h \
            ../base_classes/qffitresultsevaluation.h \
            ../base_classes/qffitresultsbyindexevaluation.h \
            ../base_classes/qffitresultsevaluationeditorbase.h \
            ../base_classes/qffitresultsbyindexevaluationeditorbase.h \
            ../interfaces/qfrdrsimplecountrateinterface.h \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.h \
    ../interfaces/qfextensiontool.h \
    ../base_classes/qffitresultsbyindexevaluationfitthread.h \
    optionswidget.h \
    ../interfaces/qfrdrrunselection.h \
    ../base_classes/qffcsweightingtools.h \
    ../base_classes/qffitresultsbyindexevaluationfittools.h \
    ../base_classes/qffcstools.h \
    ../base_classes/qffcsfitchi2landscapedialog.h \
    ../base_classes/qffitresultsbyindexevaluationfittools.h


SOURCES +=  qfpevalfcsfit.cpp \
            qffcsfitevaluation.cpp \
            qffcsfitevaluationeditor.cpp \
            ../../../../../LIB/trunk/statistics_tools.cpp \
            dlgestimatefocalvolume.cpp \
            ../base_classes/qffitresultsevaluation.cpp \
            ../base_classes/qffitresultsbyindexevaluation.cpp \
            ../base_classes/qffitresultsevaluationeditorbase.cpp\
            ../base_classes/qffitresultsbyindexevaluationeditorbase.cpp \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.cpp \
    ../base_classes/qffitresultsbyindexevaluationfitthread.cpp \
    optionswidget.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffcstools.cpp \
    ../base_classes/qffcsfitchi2landscapedialog.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp


FORMS =    estimatefocalvolumedlg.ui \
    optionswidget.ui \
    ../base_classes/qffcsfitchi2landscapedialog.ui

RESOURCES += qfevalfcsfit.qrc \
    ../base_classes/qffitresultsevaluationeditorbase.qrc

TRANSLATIONS= ./translations/de.qfevalfcsfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg core
CONFIG += exceptions rtti stl





