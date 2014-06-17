TEMPLATE = lib
CONFIG += plugin

TARGET = qfrdrfcs
DEPENDPATH += ./

include(../plugins.pri)

DEFINES += TARGETNAME=$$TARGET

include(../../libquickfitwidgets.pri)

# Input
HEADERS += dlgcsvparameters.h \
            alv5000tools.h \
            alv6000tools.h \
            qfrdrfcscorrelationeditor.h \
            qfrdrfcsdata.h \
            qfrdrfcsrateeditor.h \
            qfprdrfcs.h \
            ../../../../../LIB/trunk/qt/completertextedit.h \
            qfrdrfcsfitfunctionsimulator.h \
            ../../../../../LIB/trunk/jkmathparser.h \
            ../../../../../LIB/trunk/jkiniparser2.h \
            ../interfaces/qfrdrsimplecountrateinterface.h \
            ../base_classes/qfcorrelationmasktools.h \
            ../base_classes/qfrdrimagemasktools.h \
            ../base_classes/qffinderroneouscorrelationdlg.h \
            ../base_classes/qffcstools.h \
            ../interfaces/qfrdrrunselection.h \
            ../interfaces/qfrdrfcsdatainterface.h \
            ../interfaces/qfrdrcountratesinterface.h \
            ../interfaces/qfrdrfcsdatainterface.h \
            ../interfaces/qfrdrimageselectioninterface.h \
            ../interfaces/qfrdrrunselection.h \
    ../base_classes/qfrdrrunselectiontools.h \
    confocor3tools.h \
    flex_sin_tools.h



SOURCES += dlgcsvparameters.cpp \
           alv5000tools.cpp \
           alv6000tools.cpp \
           qfrdrfcscorrelationeditor.cpp \
           qfrdrfcsdata.cpp \
           qfrdrfcsrateeditor.cpp \
             qfprdrfcs.cpp \
    ../../../../../LIB/trunk/qt/completertextedit.cpp \
    qfrdrfcsfitfunctionsimulator.cpp \
    ../../../../../LIB/trunk/jkmathparser.cpp \
    ../../../../../LIB/trunk/jkiniparser2.cpp \
    ../base_classes/qfcorrelationmasktools.cpp \
    ../base_classes/qfrdrimagemasktools.cpp \
    ../base_classes/qffinderroneouscorrelationdlg.cpp \
    ../base_classes/qffcstools.cpp \
    ../base_classes/qfrdrrunselectiontools.cpp \
    confocor3tools.cpp \
    flex_sin_tools.cpp



FORMS = dlg_csvparameters.ui \
    qfrdrfcsfitfunctionsimulator.ui \
    ../base_classes/qffcsfitchi2landscapedialog.ui \
    ../base_classes/qffinderroneouscorrelationdlg.ui

RESOURCES += qfrdrfcs.qrc

TRANSLATIONS= ./translations/de.fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl

ASSETSTESTDATA_FILES.files = ./examples/fcs_testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/fcs_testdata/
ASSETSTESTFCCSDATA_FILES.files = ./examples/fccs_data/*.*
ASSETSTESTFCCSDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/fccs_data/

INSTALLS += ASSETSTESTDATA_FILES ASSETSTESTFCCSDATA_FILES




