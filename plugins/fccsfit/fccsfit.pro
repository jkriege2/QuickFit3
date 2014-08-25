TEMPLATE = lib
CONFIG += plugin

TARGET = fccs_fit
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qffccsfitevaluation.h \
           qffccsfitevaluation_item.h \
           qffccsfitevaluation_editor.h \
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
    qffccsparameterinputtable.h \
    qffccsparameterinputdelegate.h \
    qffccsfilesetsmodel.h \
    ../base_classes/qfrdrrunselector.h \
    dlgfccsfitallconfiguration.h \
    qffccsrelativeccfcrosstalkdialog.h \
    ../base_classes/qffitfunctionconfigforglobalfittools.h

SOURCES += qffccsfitevaluation.cpp \
           qffccsfitevaluation_item.cpp \
           qffccsfitevaluation_editor.cpp \
    ../base_classes/qffitresultsbyindexasvectorevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp \
    ../base_classes/qffitresultsevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffcstools.cpp \
    optionswidget.cpp \
    qffccsparameterinputtable.cpp \
    qffccsparameterinputdelegate.cpp \
    qffccsfilesetsmodel.cpp \
    ../base_classes/qfrdrrunselector.cpp \
    dlgfccsfitallconfiguration.cpp \
    qffccsrelativeccfcrosstalkdialog.cpp \
    ../base_classes/qffitfunctionconfigforglobalfittools.cpp

FORMS = qffccsfitevaluation_editor.ui \   
    optionswidget.ui \
    dlgfccsfitallconfiguration.ui \
    qffccsrelativeccfcrosstalkdialog.ui

RESOURCES += qffccsfitevaluation.qrc

TRANSLATIONS= ./translations/de.fccs_fit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg core opengl
CONFIG += exceptions rtti stl


ASSETS_RESULTS.files = ./examples/results/*.*
ASSETS_RESULTS.path = $${QFOUTPUT}/examples/$${TARGET}/results/

INSTALLS += ASSETS_RESULTS

