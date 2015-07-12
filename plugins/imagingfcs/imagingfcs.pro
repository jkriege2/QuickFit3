#
#    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#	
#	 last modification: $LastChangedDate$  (revision $Rev$)
#
#    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).
#
#    This software is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

TEMPLATE = lib
CONFIG += plugin

TARGET = imaging_fcs
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)
include(../../extlibs/lmfit.pri)
include(../../extlibsb040/tinytiff.pri)
include(../../extlibsb040/yaidtools.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrimagingfcs.h \
           qfrdrimagingfcs_data.h \
		   qfrdrimagingfcs_dataeditor.h \
           ../interfaces/qfrdrfcsdatainterface.h \
    ../interfaces/qfrdroverviewimageinterface.h \
    ../base_classes/qfrdrimagingfcstools.h \
    ../base_classes/qfrdrimagemasktools.h \
    ../base_classes/qfcorrelationmasktools.h \
    ../base_classes/qffinderroneouscorrelationdlg.h \
    ../base_classes/qffcstools.h \
    ../interfaces/qfrdrimagestackinterface.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../base_classes/qfrdroverviewimagedisplay.h \
    ../interfaces/qfrdrimfcscorrelatorremote.h \
    qfrdrimagingfcs_dataeditor_image.h \
    qfrdrimagingfcsrunsmodel.h \
    qfrdrimagingfcscorrelationdialog.h \
    qfrdrimagingfcsthreadprogress.h \
    qfrdrimagingfcscorrelationjobthread.h \
    ../../extlibsb040/StatisticsTools/shiftregister.h \
    ../../extlibsb040/StatisticsTools/multitau-correlator.h \
    qfrdrimagingfcsdataexplorer.h \
    qfrdrimagingfcsmaskbyintensity.h \
    qfrdrimagingfcscountratedisplay.h \
    qfrdrimagingfcsoverviewrateeditor.h \
    qfrdrimagingfcsseriesdialog.h \
    qfrdrimagingfcs_dataeditor_countrate.h \
    qfrdrimagingfcsedittools.h \
    qfrdrimagingfcssimulator.h \
    qfrdrimagingfcssimulationthread.h \
    ../../extlibs/MersenneTwister.h \
    ../base_classes/qfrdrrunselectiontools.h \
    ../base_classes/qfrdrimagecopydataselectdialog.h \
    ../base_classes/qfoverlaystylecombobox.h \
    ../base_classes/qfimageplotter.h \
    ../base_classes/qfimageparametergroupbox.h \
    qfrdrimagingfcsmaskbuilder.h \
    qfrdrimagingfcscopycorrasrdrdialog.h \
    qfrdrimagingfcspostprocessbinningdialog.h \
    qfrdrimagingfcs_parserfunctions.h \
    qfrdrimagingfcsdiffusionlawdialog.h
#    ../base_classes/frawdataimageeditor.h \



SOURCES += qfrdrimagingfcs.cpp \
           qfrdrimagingfcs_data.cpp \
		   qfrdrimagingfcs_dataeditor.cpp \
    qfrdrimagingfcs_dataeditor_image.cpp \
    qfrdrimagingfcsrunsmodel.cpp \
    qfrdrimagingfcscorrelationdialog.cpp \
    qfrdrimagingfcsthreadprogress.cpp \
    qfrdrimagingfcscorrelationjobthread.cpp \
    qfrdrimagingfcsdataexplorer.cpp \
    qfrdrimagingfcsmaskbyintensity.cpp \
    qfrdrimagingfcscountratedisplay.cpp \
    qfrdrimagingfcsoverviewrateeditor.cpp \
    ../base_classes/qfrdrimagingfcstools.cpp \
    qfrdrimagingfcsseriesdialog.cpp \
    ../base_classes/qfrdroverviewimagedisplay.cpp \
    qfrdrimagingfcs_dataeditor_countrate.cpp \
    ../base_classes/qfrdrimagemasktools.cpp\
    ../base_classes/qfcorrelationmasktools.cpp \
    ../base_classes/qffinderroneouscorrelationdlg.cpp \
    qfrdrimagingfcsedittools.cpp \
    qfrdrimagingfcssimulator.cpp \
    qfrdrimagingfcssimulationthread.cpp \
    ../base_classes/qffcstools.cpp \
    ../base_classes/qfrdrrunselectiontools.cpp \
    ../base_classes/qfrdrimagecopydataselectdialog.cpp \
    ../base_classes/qfoverlaystylecombobox.cpp \
    ../base_classes/qfimageplotter.cpp \
    ../base_classes/qfimageparametergroupbox.cpp \
    qfrdrimagingfcsmaskbuilder.cpp \
    qfrdrimagingfcscopycorrasrdrdialog.cpp \
    qfrdrimagingfcspostprocessbinningdialog.cpp \
    qfrdrimagingfcs_parserfunctions.cpp \
    qfrdrimagingfcsdiffusionlawdialog.cpp
#    ../base_classes/frawdataimageeditor.cpp \

FORMS = \
    qfrdrimagingfcscorrelationdialog.ui \
    qfrdrimagingfcsthreadprogress.ui \
    qfrdrimagingfcsdataexplorer.ui \
    qfrdrimagingfcsmaskbyintensity.ui \
    qfrdrimagingfcscountratedisplay.ui \
    qfrdrimagingfcsseriesdialog.ui \
    ../base_classes/qffinderroneouscorrelationdlg.ui \
    qfrdrimagingfcssimulator.ui \
    ../base_classes/qfrdrimagecopydataselectdialog.ui \
    qfrdrimagingfcsmaskbuilder.ui \
    qfrdrimagingfcscopycorrasrdrdialog.ui \
    qfrdrimagingfcspostprocessbinningdialog.ui \
    qfrdrimagingfcsdiffusionlawdialog.ui

RESOURCES += qfrdrimagingfcs.qrc

TRANSLATIONS= ./translations/de.imaging_fcs.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

include(../../quickfit3_configqtwidgets.pri)

#DEFINES += DEBUG_TIMIMNG JKQTBP_DEBUGTIMING


ASSETS_TESTDATA.files = ./examples/imfcs_testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/examples/$${TARGET}/imfcs_testdata/
ASSETS_TESTDATARESULTS.files = ./examples/imfcs_testdata/results/*.*
ASSETS_TESTDATARESULTS.path = $${QFOUTPUT}/examples/$${TARGET}/imfcs_testdata/results/

INSTALLS += ASSETS_TESTDATA ASSETS_TESTDATARESULTS
