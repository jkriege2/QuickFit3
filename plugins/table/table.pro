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
    qfrdrtableformuladialog.h \
    qfrdrtablesortdialog.h

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
    qfrdrtableformuladialog.cpp \
    qfrdrtablesortdialog.cpp

FORMS = dlg_csvparameters.ui \
    tableresizedialog.ui \
    qfrdrtableplotwidget.ui \
    qfrdrtablecolumnvaluesdialog.ui \
    qfrdrtableformuladialog.ui \
    qfrdrtablesortdialog.ui

RESOURCES += qfrdrtable.qrc

TRANSLATIONS= ./translations/de.qfrdrtable.ts

INCLUDEPATH += ../../lib/ \
               ../../../../../LIB/trunk/

QT += gui xml
CONFIG += exceptions rtti stl



HELPREF_FILES.files = ./help/parserreference/*.*
HELPREF_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/

HELPREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPREFPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/pic/


INSTALLS += HELPREF_FILES HELPREFPIC_FILES



