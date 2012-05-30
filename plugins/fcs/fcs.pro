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
           qfrdrfcscorrelationeditor.h \
           qfrdrfcsdata.h \
           qfrdrfcsrateeditor.h \
		   qfprdrfcs.h \
		   ../interfaces/qfrdrfcsdatainterface.h \
		   ../interfaces/qfrdrcountratesinterface.h \
    ../../../../../LIB/trunk/qt/completertextedit.h \
    qfrdrfcsfitfunctionsimulator.h

SOURCES += dlgcsvparameters.cpp \
           alv5000tools.cpp \
           qfrdrfcscorrelationeditor.cpp \
           qfrdrfcsdata.cpp \
           qfrdrfcsrateeditor.cpp \
		   qfprdrfcs.cpp \
    ../../../../../LIB/trunk/qt/completertextedit.cpp \
    qfrdrfcsfitfunctionsimulator.cpp

FORMS = dlg_csvparameters.ui \
    qfrdrfcsfitfunctionsimulator.ui

RESOURCES += qfrdrfcs.qrc

TRANSLATIONS= ./translations/de.fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl

ASSETSTESTDATA_FILES.files = ./assets/fcs_testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/fcs_testdata/
ASSETSTESTFCCSDATA_FILES.files = ./assets/fccs_data/*.*
ASSETSTESTFCCSDATA_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/fccs_data/

INSTALLS += ASSETSTESTDATA_FILES ASSETSTESTFCCSDATA_FILES




