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
    ../interfaces/qfrdrimagetoruninterface.h \
    ../interfaces/qfrdroverviewimageinterface.h \
    ../base_classes/qfrdrimagingfcstools.h \
    ../base_classes/qfrdrimagemasktools.h \
    ../base_classes/qfcorrelationmasktools.h \
    ../base_classes/qffinderroneouscorrelationdlg.h \
    ../base_classes/qffcstools.h \
    ../interfaces/qfrdrimagestackinterface.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../interfaces/qfrdrimageselectioninterface.h \
    ../base_classes/qfrdroverviewimagedisplay.h \
    ../interfaces/qfrdrrunselection.h \
    ../interfaces/qfrdrimagemask.h \
    ../interfaces/qfrdrimfcscorrelatorremote.h \
    qfrdrimagingfcs_dataeditor_image.h \
    qfrdrimagingfcsrunsmodel.h \
    qfrdrimagingfcscorrelationdialog.h \
    qfrdrimagingfcsthreadprogress.h \
    qfrdrimagingfcscorrelationjobthread.h \
    ../../../../../LIB/trunk/tinytiffwriter.h \
    ../../../../../LIB/trunk/libtiff_tools.h \
    ../../../../../LIB/trunk/shiftregister.h \
    ../../../../../LIB/trunk/multitau-correlator.h \
    ../../../../../LIB/trunk/correlator_multitau.h \
    qfrdrimagingfcsdataexplorer.h \
    qfrdrimagingfcsmaskbyintensity.h \
    qfrdrimagingfcscountratedisplay.h \
    qfrdrimagingfcsoverviewrateeditor.h \
    ../../../../../LIB/trunk/yaid_rh.h \
    qfrdrimagingfcsseriesdialog.h \
    qfrdrimagingfcscopydataselectdialog.h \
    qfrdrimagingfcs_dataeditor_countrate.h \
    qfrdrimagingfcsedittools.h \
    qfrdrimagingfcssimulator.h \
    qfrdrimagingfcssimulationthread.h \
    ../../extlibs/MersenneTwister.h \
    qfrdrimagingfcsimageplotter.h \
    qfrdrimagingfcsimageparametergroupbox.h \
    qfrdrimagingfcsoverlaystylecombobox.h \
    ../base_classes/qfrdrrunselectiontools.h



SOURCES += qfrdrimagingfcs.cpp \
           qfrdrimagingfcs_data.cpp \
		   qfrdrimagingfcs_dataeditor.cpp \
    qfrdrimagingfcs_dataeditor_image.cpp \
    qfrdrimagingfcsrunsmodel.cpp \
    qfrdrimagingfcscorrelationdialog.cpp \
    qfrdrimagingfcsthreadprogress.cpp \
    qfrdrimagingfcscorrelationjobthread.cpp \
    ../../../../../LIB/trunk/tinytiffwriter.cpp \
    ../../../../../LIB/trunk/libtiff_tools.cpp \
    qfrdrimagingfcsdataexplorer.cpp \
    qfrdrimagingfcsmaskbyintensity.cpp \
    qfrdrimagingfcscountratedisplay.cpp \
    qfrdrimagingfcsoverviewrateeditor.cpp \
    ../base_classes/qfrdrimagingfcstools.cpp \
    qfrdrimagingfcsseriesdialog.cpp \
    qfrdrimagingfcscopydataselectdialog.cpp \
    ../base_classes/qfrdroverviewimagedisplay.cpp \
    qfrdrimagingfcs_dataeditor_countrate.cpp \
    ../base_classes/qfrdrimagemasktools.cpp\
    ../base_classes/qfcorrelationmasktools.cpp \
    ../base_classes/qffinderroneouscorrelationdlg.cpp \
    qfrdrimagingfcsedittools.cpp \
    qfrdrimagingfcssimulator.cpp \
    qfrdrimagingfcssimulationthread.cpp \
    ../base_classes/qffcstools.cpp \
    qfrdrimagingfcsimageplotter.cpp \
    qfrdrimagingfcsimageparametergroupbox.cpp \
    qfrdrimagingfcsoverlaystylecombobox.cpp \
    ../base_classes/qfrdrrunselectiontools.cpp

FORMS = \
    qfrdrimagingfcscorrelationdialog.ui \
    qfrdrimagingfcsthreadprogress.ui \
    qfrdrimagingfcsdataexplorer.ui \
    qfrdrimagingfcsmaskbyintensity.ui \
    qfrdrimagingfcscountratedisplay.ui \
    qfrdrimagingfcsseriesdialog.ui \
    qfrdrimagingfcscopydataselectdialog.ui \
    ../base_classes/qffinderroneouscorrelationdlg.ui \
    qfrdrimagingfcssimulator.ui

RESOURCES += qfrdrimagingfcs.qrc

TRANSLATIONS= ./translations/de.imaging_fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg opengl
CONFIG += exceptions rtti stl


ASSETS_TESTDATA.files = ./examples/imfcs_testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/examples/$${TARGET}/imfcs_testdata/

INSTALLS += ASSETS_TESTDATA
