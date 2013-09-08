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
    ../base_classes/qffitresultsevaluation.h \
    ../base_classes/qffitresultsbyindexevaluation.h \
    ../base_classes/qffitresultsevaluationeditorbase.h \
    ../base_classes/qffitresultsbyindexevaluationeditorbase.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../interfaces/qfrdrimagetoruninterface.h \
    ../base_classes/qffitresultsbyindexasvectorevaluation.h \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.h \
    imfcscalibrationdialog.h \
    ../base_classes/qffitresultsbyindexevaluationfitthread.h \
    optionswidget.h \
    ../interfaces/qfrdrrunselection.h \
    ../base_classes/qffcsweightingtools.h \
    ../base_classes/qffitresultsbyindexevaluationfittools.h \
    ../base_classes/qffcstools.h \
    ../base_classes/qfrdrimagetorunpreview.h \
    qfimfcssetparamfromfiledialog.h \
    imfcscalibrationwizard.h \
    ../interfaces/qfrdrimfcscorrelatorremote.h \
    ../base_classes/qffcsfitchi2landscapedialog.h


SOURCES += qfpevalimfcsfit.cpp \
           qfimfcsfitevaluation.cpp \
           qfimfcsfitevaluationeditor.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp \
    ../base_classes/qffitresultsevaluationeditorbase.cpp\
    ../base_classes/qffitresultsbyindexevaluationeditorbase.cpp \
    ../base_classes/qffitresultsevaluation.cpp \
    ../base_classes/qffitresultsbyindexasvectorevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.cpp \
    imfcscalibrationdialog.cpp \
    ../base_classes/qffitresultsbyindexevaluationfitthread.cpp \
    optionswidget.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp \
    ../base_classes/qffcstools.cpp \
    ../base_classes/qfrdrimagetorunpreview.cpp \
    qfimfcssetparamfromfiledialog.cpp \
    imfcscalibrationwizard.cpp \
    ../base_classes/qffcsfitchi2landscapedialog.cpp


FORMS = \    
    imfcscalibrationdialog.ui \
    optionswidget.ui \
    qfimfcssetparamfromfiledialog.ui \
    imfcscalibrationwizard.ui \
    ../base_classes/qffcsfitchi2landscapedialog.ui

RESOURCES += qfevalimfcsfit.qrc \
    ../base_classes/qffitresultsevaluationeditorbase.qrc

TRANSLATIONS= ./translations/de.qfevalimfcsfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg core opengl
CONFIG += exceptions rtti stl
