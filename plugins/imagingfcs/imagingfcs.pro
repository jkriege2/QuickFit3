TEMPLATE = lib
CONFIG += plugin

TARGET = imaging_fcs
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)
include(../../extlibs/lmfit.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrimagingfcs.h \
           qfrdrimagingfcs_data.h \
		   qfrdrimagingfcs_dataeditor.h \
           ../interfaces/qfrdrfcsdatainterface.h \
    qfrdrimagingfcs_dataeditor_image.h \
    qfrdrimagingfcsrunsmodel.h \
    ../base_classes/qftablemodel.h \
    qfrdrimagingfcscorrelationdialog.h \
    qfrdrimagingfcsthreadprogress.h \
    qfrdrimagingfcscorrelationjobthread.h \
    ../../../../../LIB/trunk/tinytiffwriter.h \
    ../../../../../LIB/trunk/libtiff_tools.h \
    ../../../../../LIB/trunk/statistics_tools.h \
    ../../../../../LIB/trunk/shiftregister.h \
    ../../../../../LIB/trunk/multitau-correlator.h \
    ../../../../../LIB/trunk/correlator_multitau.h \
    ../base_classes/qfhistogramview.h \
    qfrdrimagingfcsdataexplorer.h \
    qfrdrimagingfcsmaskbyintensity.h \
    qfrdrimagingfcscountratedisplay.h \
    ../interfaces/qfrdrimagetoruninterface.h \
    ../interfaces/qfrdroverviewimageinterface.h \
    qfrdrimagingfcsoverviewimagedisplay.h \
    qfrdrimagingfcsoverviewrateeditor.h \
    qfrdrimagingfcstools.h \
    ../interfaces/qfrdrimagestackinterface.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../../../../../LIB/trunk/yaid_rh.h \
    qfrdrimagingfcsseriesdialog.h \
    qfrdrimagingfcscopydataselectdialog.h \
    ../interfaces/qfrdrimageselectioninterface.h


SOURCES += qfrdrimagingfcs.cpp \
           qfrdrimagingfcs_data.cpp \
		   qfrdrimagingfcs_dataeditor.cpp \
    qfrdrimagingfcs_dataeditor_image.cpp \
    qfrdrimagingfcsrunsmodel.cpp \
    ../base_classes/qftablemodel.cpp \
    qfrdrimagingfcscorrelationdialog.cpp \
    qfrdrimagingfcsthreadprogress.cpp \
    qfrdrimagingfcscorrelationjobthread.cpp \
    ../../../../../LIB/trunk/tinytiffwriter.cpp \
    ../../../../../LIB/trunk/libtiff_tools.cpp \
    ../../../../../LIB/trunk/statistics_tools.cpp \
    ../base_classes/qfhistogramview.cpp \
    qfrdrimagingfcsdataexplorer.cpp \
    qfrdrimagingfcsmaskbyintensity.cpp \
    qfrdrimagingfcscountratedisplay.cpp \
    qfrdrimagingfcsoverviewimagedisplay.cpp \
    qfrdrimagingfcsoverviewrateeditor.cpp \
    qfrdrimagingfcstools.cpp \
    qfrdrimagingfcsseriesdialog.cpp \
    qfrdrimagingfcscopydataselectdialog.cpp

FORMS = \
    qfrdrimagingfcscorrelationdialog.ui \
    qfrdrimagingfcsthreadprogress.ui \
    qfrdrimagingfcsdataexplorer.ui \
    qfrdrimagingfcsmaskbyintensity.ui \
    qfrdrimagingfcscountratedisplay.ui \
    qfrdrimagingfcsseriesdialog.ui \
    qfrdrimagingfcscopydataselectdialog.ui

RESOURCES += qfrdrimagingfcs.qrc

TRANSLATIONS= ./translations/de.imaging_fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg opengl
CONFIG += exceptions rtti stl


ASSETS_TESTDATA.files = ./assets/imfcs_testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/imfcs_testdata/

INSTALLS += ASSETS_TESTDATA
