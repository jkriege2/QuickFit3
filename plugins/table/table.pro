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
    qfrdrtableplotwidget.h \
    qftablepluginmodel.h \
    qfrdrtablecolumnvaluesdialog.h \
    ../../../../../LIB/trunk/jkmathparser.h \
    qfrdrtableformuladialog.h

SOURCES += dlgcsvparameters.cpp \
           qfrdrtable.cpp \
           qfrdrtableeditor.cpp \
           ../base_classes/qftablemodel.cpp \
           qfprdrtable.cpp \
    tableresizedialog.cpp \
    qfrdrtabledelegate.cpp \
    qfrdrploteditor.cpp \
    qfrdrtableplotwidget.cpp \
    qftablepluginmodel.cpp \
    qfrdrtablecolumnvaluesdialog.cpp \
    ../../../../../LIB/trunk/jkmathparser.cpp \
    qfrdrtableformuladialog.cpp

FORMS = dlg_csvparameters.ui \
    tableresizedialog.ui \
    qfrdrtableplotwidget.ui \
    qfrdrtablecolumnvaluesdialog.ui \
    qfrdrtableformuladialog.ui

RESOURCES += qfrdrtable.qrc

TRANSLATIONS= ./translations/de.qfrdrtable.ts

INCLUDEPATH += ../../lib/ \
               ../../../../../LIB/trunk/

QT += gui xml
CONFIG += exceptions rtti stl




