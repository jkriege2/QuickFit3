TEMPLATE = lib
CONFIG += plugin


TARGET = spimb040
DEPENDPATH += ./


include(../plugins.pri)
include(../../extlibs/lmfit.pri)
include(../../extlibs/tiff.pri)

DESTDIR = $$QFOUTPUT/plugins
include(../../libquickfitwidgets.pri)

# Input
HEADERS += spimb040.h \
           qfespimb040cameraview.h \
           qfespimb040samplestageconfig.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/jkimage.h  \
           ../../../../../LIB/trunk/highrestimer.h \
           ../../../../../LIB/trunk/tinytiffwriter.h \
           ../../../../../LIB/trunk/libtiff_tools.h \
           ../interfaces/qfextensionlinearstage.h \
           ../interfaces/qfextensionparameterdevice.h \
    qfespimb040opticssetup.h \
    qfespimb040simplecameraconfig.h \
    qfespimb00histogrammaskdialog.h \
    ../../../../../LIB/trunk/qt/qfasttablelabel.h \
    ../../../../../LIB/trunk/histogram.h \
    qfespimb040mainwindow2.h \
    qfespimb040experimentdescription.h \
    qfespimb040samplestageconfigthread.h \
    qfespimb040imagestackconfigwidget2.h \
    qfespimb040acquisitionconfigwidget2.h \
    qfespimb040configcombobox.h \
    qfespimb040samplestagestepconfigdialog.h \
    qfespimb040shortcutconfigdialog.h \
    qfespimb040camparamstackconfigwidget2.h \
    qfespimb040filenametool.h \
    qfespimb040lightpathsavedialog.h \
    ../interfaces/qfextensionmeasurementdevice.h \
    qfespimb040acquisitiondescription.h \
    ../interfaces/qfextensionglobalsettingsreadwrite.h \
    qfespimb040deviceparamstackconfigwidget.h \
    qfespimb040acquisitiontools.h \
    qfespimb040scriptedacquisition.h \
    qfespimb040scriptedacquisitiontools.h \
    replacedialog.h \
    finddialog.h \
    qfespimb040configtabwidget.h \
    qfespimb040opticssetupbase.h \
    qfespimb040opticssetup2.h \
    ../../../../../LIB/trunk/jkmathparser.h \
    ../../../../../LIB/trunk/jkiniparser2.h

SOURCES += spimb040.cpp \
           qfespimb040cameraview.cpp \
           qfespimb040samplestageconfig.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp \
           ../../../../../LIB/trunk/tinytiffwriter.cpp \
           ../../../../../LIB/trunk/libtiff_tools.cpp \
    qfespimb040opticssetup.cpp \
    qfespimb040simplecameraconfig.cpp \
    qfespimb00histogrammaskdialog.cpp \
    ../../../../../LIB/trunk/qt/qfasttablelabel.cpp \
    qfespimb040mainwindow2.cpp \
    qfespimb040experimentdescription.cpp \
    qfespimb040samplestageconfigthread.cpp \
    qfespimb040imagestackconfigwidget2.cpp \
    qfespimb040acquisitionconfigwidget2.cpp \
    qfespimb040configcombobox.cpp \
    qfespimb040samplestagestepconfigdialog.cpp \
    qfespimb040shortcutconfigdialog.cpp \
    qfespimb040camparamstackconfigwidget2.cpp \
    qfespimb040filenametool.cpp \
    qfespimb040lightpathsavedialog.cpp \
    qfespimb040acquisitiondescription.cpp \
    qfespimb040deviceparamstackconfigwidget.cpp \
    qfespimb040scriptedacquisition.cpp \
    qfespimb040scriptedacquisitiontools.cpp \
    replacedialog.cpp \
    finddialog.cpp \
    qfespimb040configtabwidget.cpp \
    qfespimb040opticssetupbase.cpp \
    qfespimb040opticssetup2.cpp \
    ../../../../../LIB/trunk/jkmathparser.cpp \
    ../../../../../LIB/trunk/jkiniparser2.cpp
FORMS = \
    qfespimb040opticssetup.ui \
    qfespimb00histogrammaskdialog.ui \
    qfespimb040experimentdescription.ui \
    qfespimb040imagestackconfigwidget2.ui \
    qfespimb040acquisitionconfigwidget2.ui \
    qfespimb040samplestagestepconfigdialog.ui \
    qfespimb040shortcutconfigdialog.ui \
    qfespimb040camparamstackconfigwidget2.ui \
    qfespimb040lightpathsavedialog.ui \
    qfespimb040acquisitiondescription.ui \
    qfespimb040deviceparamstackconfigwidget.ui \
    qfespimb040scriptedacquisition.ui \
    replacedialog.ui \
    finddialog.ui \
    qfespimb040configtabwidget.ui \
    qfespimb040opticssetup2.ui

RESOURCES += spimb040.qrc

TRANSLATIONS= ./translations/de.spimb040.ts

INCLUDEPATH += . \
               ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg opengl script

CONFIG += exceptions rtti stl



ASSETSScriptTemplates_FILES.files = ./assets/acquisitionScriptTemplates/*.*
ASSETSScriptTemplates_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/acquisitionScriptTemplates

ASSETSScriptExamples_FILES.files = ./assets/acquisitionScriptExamples/*.*
ASSETSScriptExamples_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/acquisitionScriptExamples

ASSETSScriptHELP_FILES.files = ./help/acquisition_script/*.*
ASSETSScriptHELP_FILES.path =    $${QFOUTPUT}/assets/plugins/help/$${TARGET}/acquisition_script

ASSETSScriptHELPPIC_FILES.files = ./help/acquisition_script/pic/*.*
ASSETSScriptHELPPIC_FILES.path =    $${QFOUTPUT}/assets/plugins/help/$${TARGET}/acquisition_script/pic

ASSETSSTYLESHEETS_FILES.files = ./assets/stylesheets/*.*
ASSETSSTYLESHEETS_FILES.path =    $${QFOUTPUT}/assets/plugins/$${TARGET}/stylesheets

ASSETSSTYLESHEETSDO_FILES.files = ./assets/stylesheets/dark_orange/*.*
ASSETSSTYLESHEETSDO_FILES.path =    $${QFOUTPUT}/assets/plugins/$${TARGET}/stylesheets/dark_orange


INSTALLS += ASSETSScriptTemplates_FILES ASSETSScriptExamples_FILES ASSETSScriptHELP_FILES ASSETSScriptHELPPIC_FILES ASSETSSTYLESHEETS_FILES ASSETSSTYLESHEETSDO_FILES




















