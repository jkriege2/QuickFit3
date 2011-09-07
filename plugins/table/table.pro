TEMPLATE = lib
CONFIG += plugin


TARGET = qfrdrtable
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
LIBS += -L$$QFOUTPUT -lquickfit3lib -L$$QFOUTPUT -lquickfit3widgets

# Input
HEADERS += dlgcsvparameters.h \
           qfrdrtable.h \
           qfrdrtableeditor.h \
           ../base_classes/qftablemodel.h \
           qfprdrtable.h

SOURCES += dlgcsvparameters.cpp \
           qfrdrtable.cpp \
           qfrdrtableeditor.cpp \
           ../base_classes/qftablemodel.cpp \
           qfprdrtable.cpp

FORMS = dlg_csvparameters.ui

RESOURCES += qfrdrtable.qrc

TRANSLATIONS= ./translations/de.qfrdrtable.ts

INCLUDEPATH += ../../lib/ \
               ../../../../../LIB/trunk/

QT += gui xml
CONFIG += exceptions rtti stl




