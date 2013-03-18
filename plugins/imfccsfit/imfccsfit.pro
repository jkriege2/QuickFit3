TEMPLATE = lib
CONFIG += plugin

TARGET = imfccs_fit
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfimfccsfitevaluation.h \
           qfimfccsfitevaluation_item.h \
           qfimfccsfitevaluation_editor.h \
    ../base_classes/qffitresultsbyindexasvectorevaluation.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../interfaces/qfrdrfcsdatainterface.h \
    ../interfaces/qfrdrcountratesinterface.h \
    ../base_classes/qffitresultsbyindexevaluation.h \
    ../base_classes/qffitresultsevaluation.h \
    ../base_classes/qffitresultsbyindexevaluationfittools.h \
    ../base_classes/qffcsweightingtools.h

SOURCES += qfimfccsfitevaluation.cpp \
           qfimfccsfitevaluation_item.cpp \
           qfimfccsfitevaluation_editor.cpp \
    ../base_classes/qffitresultsbyindexasvectorevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp \
    ../base_classes/qffitresultsevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp \
    ../base_classes/qffcsweightingtools.cpp

FORMS = qfimfccsfitevaluation_editor.ui   

RESOURCES += qfimfccsfitevaluation.qrc

TRANSLATIONS= ./translations/de.imfccs_fit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl



