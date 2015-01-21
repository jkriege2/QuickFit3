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
            ../interfaces/qfrdrfcsdatainterface.h \
            ../interfaces/qfrdrcountratesinterface.h \
            ../interfaces/qfrdrfcsdatainterface.h \
    ../base_classes/qfrdrrunselectiontools.h \
    confocor3tools.h \
    flex_sin_tools.h \
    alv7000tools.h \
    ../base_classes/qf3correlationdataformattool.h \
    qfrdrfcsparserfunctions.h \
    qfrdrfcscrosscorrelationeditor.h



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
    flex_sin_tools.cpp \
    alv7000tools.cpp \
    ../base_classes/qf3correlationdataformattool.cpp \
    qfrdrfcsparserfunctions.cpp \
    qfrdrfcscrosscorrelationeditor.cpp



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

include(../../quickfit3_configqtwidgets.pri)

ASSETSTESTDATA_FILES.files = ./examples/fcs_testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/fcs_testdata/
ASSETSTESTDLSDATA_FILES.files = ./examples/dls_testdata/*.*
ASSETSTESTDLSDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/dls_testdata/
ASSETSTESTFCCSDATA_FILES.files = ./examples/fccs_data/*.*
ASSETSTESTFCCSDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/fccs_data/
ASSETSTESTALV7000DATA_FILES.files = ./examples/ALV7000_testdata/*.*
ASSETSTESTALV7000DATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/ALV7000_testdata/

INSTALLS += ASSETSTESTDATA_FILES ASSETSTESTFCCSDATA_FILES ASSETSTESTDLSDATA_FILES ASSETSTESTALV7000DATA_FILES




