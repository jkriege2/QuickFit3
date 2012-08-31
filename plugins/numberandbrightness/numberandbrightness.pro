TEMPLATE = lib
CONFIG += plugin


TARGET = number_and_brightness
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrnumberandbrightness.h \
           qfrdrnumberandbrightness_data.h \
		   qfrdrnumberandbrightness_dataeditor.h \
    ../base_classes/qfrdrimagingfcstools.h \
    ../../../../../LIB/trunk/libtiff_tools.h \
    qfrdrnumberandbrightness_overvieweditor.h \
    ../base_classes/qfrdroverviewimagedisplay.h

SOURCES += qfrdrnumberandbrightness.cpp \
           qfrdrnumberandbrightness_data.cpp \
		   qfrdrnumberandbrightness_dataeditor.cpp \
    ../base_classes/qfrdrimagingfcstools.cpp \
    ../../../../../LIB/trunk/libtiff_tools.cpp \
    qfrdrnumberandbrightness_overvieweditor.cpp \
    ../base_classes/qfrdroverviewimagedisplay.cpp

FORMS = 

RESOURCES += numberandbrightness.qrc

TRANSLATIONS= ./translations/de.number_and_brightness.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core opengl
CONFIG += exceptions rtti stl


ASSETS_TESTDATA_FILES.files = ./assets/test_data/*.*
ASSETS_TESTDATA_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/test_data

INSTALLS += ASSETS_TESTDATA_FILES

