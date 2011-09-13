TEMPLATE = lib
CONFIG += plugin

TARGET = qfevalfcsfit
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += ../base_classes/qffitresultsevaluation.h \
           qfpevalfcsfit.h \
           qffcsfitevaluation.h \
           qffcsfitevaluationeditor.h \
           ../interfaces/qfrdrfcsdatainterface.h \
		   ../interfaces/qfrdrcountratesinterface.h \
           ../../../../../LIB/trunk/statistics_tools.h \
           dlgestimatefocalvolume.h \
    ../base_classes/qffitresultsbyindexevaluation.h


SOURCES += ../base_classes/qffitresultsevaluation.cpp \
           qfpevalfcsfit.cpp \
           qffcsfitevaluation.cpp \
           qffcsfitevaluationeditor.cpp \
           ../../../../../LIB/trunk/statistics_tools.cpp \
           dlgestimatefocalvolume.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp


FORMS =    estimatefocalvolumedlg.ui

RESOURCES += qfevalfcsfit.qrc

TRANSLATIONS= ./translations/de.qfevalfcsfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg core
CONFIG += exceptions rtti stl


