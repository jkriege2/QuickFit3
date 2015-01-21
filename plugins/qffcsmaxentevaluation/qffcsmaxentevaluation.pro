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

TARGET = fcs_maxent
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)
include(../../extlibs/eigen.pri)

# Input
HEADERS += qffcsmaxentevaluation.h \
           qffcsmaxentevaluation_item.h \
           qffcsmaxentevaluation_editor.h \
    ../base_classes/qfusesresultsevaluation.h \
    ../base_classes/qfusesresultsbyindexevaluation.h \
    ../interfaces/qfrdrfcsdatainterface.h \
    ../base_classes/qfusesresultsbyindexandmodelevaluation.h \
    ../base_classes/qfusesresultsbyindexevaluationeditor.h \
    ../base_classes/qffcsbyindexandmodelevaluationeditor.h \
    libb040mem.h \
    ../base_classes/qffcsweightingtools.h \
    ../base_classes/qffcstools.h \
    ../base_classes/imfcstools.h

SOURCES += qffcsmaxentevaluation.cpp \
           qffcsmaxentevaluation_item.cpp \
           qffcsmaxentevaluation_editor.cpp \
    ../base_classes/qfusesresultsevaluation.cpp \
    ../base_classes/qfusesresultsbyindexevaluation.cpp \
    ../base_classes/qfusesresultsbyindexandmodelevaluation.cpp \
    ../base_classes/qfusesresultsbyindexevaluationeditor.cpp \
    ../base_classes/qffcsbyindexandmodelevaluationeditor.cpp \
    libb040mem.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffcstools.cpp \
    ../base_classes/imfcstools.cpp

FORMS =   

RESOURCES += qffcsmaxentevaluation.qrc

TRANSLATIONS= ./translations/de.fcs_maxent.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

include(../../quickfit3_configqtwidgets.pri)


ASSETSTESTDATA_FILES.files = ./assets/examples/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/testdata/


INSTALLS += ASSETSTESTDATA_FILES


