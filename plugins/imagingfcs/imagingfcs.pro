TEMPLATE = lib
CONFIG += plugin

QFOUTPUT = ../../output/
DESTDIR = $$QFOUTPUT/plugins

TARGET = imaging_fcs
DEPENDPATH += ./

LIBS += -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += qfrdrimagingfcs.h \
           qfrdrimagingfcs_data.h \
		   qfrdrimagingfcs_dataeditor.h \
           ../interfaces/qfrdrfcsdatainterface.h \


SOURCES += qfrdrimagingfcs.cpp \
           qfrdrimagingfcs_data.cpp \
		   qfrdrimagingfcs_dataeditor.cpp

FORMS =

RESOURCES += qfrdrimagingfcs.qrc

TRANSLATIONS= ./translations/de.imaging_fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/

ASSETS_TESTDATA.files = ./assets/imfcs_testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/assets/imfcs_testdata/

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES ASSETS_TRANSLATIONS ASSETS_TESTDATA

#POST_TARGETDEPS += install


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

