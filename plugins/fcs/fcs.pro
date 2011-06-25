TEMPLATE = lib
CONFIG += plugin

TARGET = qfrdrfcs
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

LIBS += -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += dlgcsvparameters.h \
           alv5000tools.h \
           qfrdrfcscorrelationeditor.h \
           qfrdrfcsdata.h \
           qfrdrfcsrateeditor.h \
		   qfprdrfcs.h \
		   ../interfaces/qfrdrfcsdatainterface.h \
		   ../interfaces/qfrdrcountratesinterface.h

SOURCES += dlgcsvparameters.cpp \
           alv5000tools.cpp \
           qfrdrfcscorrelationeditor.cpp \
           qfrdrfcsdata.cpp \
           qfrdrfcsrateeditor.cpp \
		   qfprdrfcs.cpp

FORMS = dlg_csvparameters.ui

RESOURCES += qfrdrfcs.qrc

TRANSLATIONS= ./translations/de.fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/

QT += gui xml svg
CONFIG += exceptions rtti stl

ASSETSTESTDATA_FILES.files = ./assets/fcs_testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/fcs_testdata/

INSTALLS += ASSETSTESTDATA_FILES


