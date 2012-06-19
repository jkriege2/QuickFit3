TEMPLATE = lib
CONFIG += plugin

TARGET = imfcsfit
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfpevalimfcsfit.h \
           qfimfcsfitevaluation.h \
           qfimfcsfitevaluationeditor.h \
           ../interfaces/qfrdrfcsdatainterface.h \
		   ../interfaces/qfrdrcountratesinterface.h \
           ../../../../../LIB/trunk/statistics_tools.h \
    ../base_classes/qffitresultsevaluation.h \
    ../base_classes/qffitresultsbyindexevaluation.h \
    ../base_classes/qffitresultsevaluationeditorbase.h \
    ../base_classes/qffitresultsbyindexevaluationeditorbase.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../interfaces/qfrdrimagetoruninterface.h \
    ../base_classes/qffitresultsbyindexasvectorevaluation.h \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.h \
    imfcscalibrationdialog.h


SOURCES += qfpevalimfcsfit.cpp \
           qfimfcsfitevaluation.cpp \
           qfimfcsfitevaluationeditor.cpp \
           ../../../../../LIB/trunk/statistics_tools.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp \
    ../base_classes/qffitresultsevaluationeditorbase.cpp\
    ../base_classes/qffitresultsbyindexevaluationeditorbase.cpp \
    ../base_classes/qffitresultsevaluation.cpp \
    ../base_classes/qffitresultsbyindexasvectorevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.cpp \
    imfcscalibrationdialog.cpp


FORMS = \    
    imfcscalibrationdialog.ui

RESOURCES += qfevalimfcsfit.qrc \
    ../base_classes/qffitresultsevaluationeditorbase.qrc

TRANSLATIONS= ./translations/de.qfevalimfcsfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg core
CONFIG += exceptions rtti stl
