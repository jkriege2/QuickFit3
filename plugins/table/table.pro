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
           qfprdrtable.h \
    tableresizedialog.h \
    qfrdrtabledelegate.h \
    qfrdrploteditor.h \
    qfrdrtableplotwidget.h \
    qftablepluginmodel.h \
    qfrdrtablecolumnvaluesdialog.h \
    ../../../../../LIB/trunk/jkmathparser.h \
    qfrdrtableformuladialog.h \
    qfrdrtablesortdialog.h \
    qfrdrtablehistogramdialog.h \
    qfrdrtableparserfunctions.h \
    ../base_classes/qffunctionreferencetool.h \
    qfrdrtableenhancedtableview.h \
    ../interfaces/qfrdrtableinterface.h \
    ../interfaces/qfrdrcolumngraphsinterface.h \
    qftablegraphsettings.h \
    qfrdrtablecolumneditor.h \
    qfrdrtablecurvefitdialog.h \
    qfrdrtableregressiondialog.h \
    dlgcolorbarcoloring.h \
    qfrdrtable2dhistogramdialog.h \
    qfrdrtableplotsettingswidget.h \
    qfrdrtableaxissettingswidget.h

SOURCES += dlgcsvparameters.cpp \
           qfrdrtable.cpp \
           qfrdrtableeditor.cpp \
           qfprdrtable.cpp \
    tableresizedialog.cpp \
    qfrdrtabledelegate.cpp \
    qfrdrploteditor.cpp \
    qfrdrtableplotwidget.cpp \
    qftablepluginmodel.cpp \
    qfrdrtablecolumnvaluesdialog.cpp \
    ../../../../../LIB/trunk/jkmathparser.cpp \
    qfrdrtableformuladialog.cpp \
    qfrdrtablesortdialog.cpp \
    qfrdrtablehistogramdialog.cpp \
    qfrdrtableparserfunctions.cpp \
    ../base_classes/qffunctionreferencetool.cpp \
    qfrdrtableenhancedtableview.cpp \
    qftablegraphsettings.cpp \
    qfrdrtablecolumneditor.cpp \
    qfrdrtablecurvefitdialog.cpp \
    qfrdrtableregressiondialog.cpp \
    dlgcolorbarcoloring.cpp \
    qfrdrtable2dhistogramdialog.cpp \
    qfrdrtableplotsettingswidget.cpp \
    qfrdrtableaxissettingswidget.cpp

FORMS = dlg_csvparameters.ui \
    tableresizedialog.ui \
    qfrdrtableplotwidget.ui \
    qfrdrtablecolumnvaluesdialog.ui \
    qfrdrtableformuladialog.ui \
    qfrdrtablesortdialog.ui \
    qfrdrtablehistogramdialog.ui \
    qftablegraphsettings.ui \
    qfrdrtablecolumneditor.ui \
    qfrdrtablecurvefitdialog.ui \
    qfrdrtableregressiondialog.ui \
    dlgcolorbarcoloring.ui \
    qfrdrtable2dhistogramdialog.ui \
    qfrdrtableplotsettingswidget.ui \
    qfrdrtableaxissettingswidget.ui

RESOURCES += qfrdrtable.qrc \
    ../../lib/lib.qrc

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



