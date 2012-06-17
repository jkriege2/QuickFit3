TEMPLATE = lib
CONFIG += plugin


TARGET = qfrdrtable
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
include(../../libquickfitwidgets.pri)

# Input
HEADERS += dlgcsvparameters.h \
           qfrdrtable.h \
           qfrdrtableeditor.h \
           ../base_classes/qftablemodel.h \
           qfprdrtable.h \
    tableresizedialog.h \
    qfrdrtabledelegate.h \
    qfrdrploteditor.h \
    qfrdrtableplotwidget.h

SOURCES += dlgcsvparameters.cpp \
           qfrdrtable.cpp \
           qfrdrtableeditor.cpp \
           ../base_classes/qftablemodel.cpp \
           qfprdrtable.cpp \
    tableresizedialog.cpp \
    qfrdrtabledelegate.cpp \
    qfrdrploteditor.cpp \
    qfrdrtableplotwidget.cpp

FORMS = dlg_csvparameters.ui \
    tableresizedialog.ui \
    qfrdrtableplotwidget.ui

RESOURCES += qfrdrtable.qrc

TRANSLATIONS= ./translations/de.qfrdrtable.ts

INCLUDEPATH += ../../lib/ \
               ../../../../../LIB/trunk/

QT += gui xml
CONFIG += exceptions rtti stl




