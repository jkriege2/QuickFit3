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
    ../base_classes/qffcsweightingtools.h \
    ../base_classes/qffcstools.h \
    optionswidget.h \
    ../base_classes/qfrdrimagetorunpreview.h \
    qfimfccsparameterinputtable.h \
    qfimfccsparameterinputdelegate.h \
    qfimfccsrelativeccfdialog.h \
    qfimfccsfilesetsmodel.h

SOURCES += qfimfccsfitevaluation.cpp \
           qfimfccsfitevaluation_item.cpp \
           qfimfccsfitevaluation_editor.cpp \
    ../base_classes/qffitresultsbyindexasvectorevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp \
    ../base_classes/qffitresultsevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffcstools.cpp \
    optionswidget.cpp \
    ../base_classes/qfrdrimagetorunpreview.cpp \
    qfimfccsparameterinputtable.cpp \
    qfimfccsparameterinputdelegate.cpp \
    qfimfccsrelativeccfdialog.cpp \
    qfimfccsfilesetsmodel.cpp

FORMS = qfimfccsfitevaluation_editor.ui \   
    optionswidget.ui \
    qfimfccsrelativeccfdialog.ui

RESOURCES += qfimfccsfitevaluation.qrc \
    ../base_classes/qfrdrmaskeditor.qrc

TRANSLATIONS= ./translations/de.imfccs_fit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg core opengl
CONFIG += exceptions rtti stl


ASSETS_RESULTS.files = ./examples/results/*.*
ASSETS_RESULTS.path = $${QFOUTPUT}/examples/$${TARGET}/results/

INSTALLS += ASSETS_RESULTS

