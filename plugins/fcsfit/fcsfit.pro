#
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#	
#	 
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

TARGET = qfevalfcsfit
DEPENDPATH += ./

include(../../qf3plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS +=  qfpevalfcsfit.h \
            qffcsfitevaluation.h \
            qffcsfitevaluationeditor.h \
            ../interfaces/qfrdrfcsdatainterface.h \
            ../interfaces/qfrdrcountratesinterface.h \
            dlgestimatefocalvolume.h \
            ../base_classes/qffitresultsevaluation.h \
            ../base_classes/qffitresultsbyindexevaluation.h \
            ../base_classes/qffitresultsevaluationeditorbase.h \
            ../base_classes/qffitresultsbyindexevaluationeditorbase.h \
            ../interfaces/qfrdrsimplecountrateinterface.h \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.h \
    ../interfaces/qfextensiontool.h \
    ../base_classes/qffitresultsbyindexevaluationfitthread.h \
    optionswidget.h \
    ../base_classes/qffcsweightingtools.h \
    ../base_classes/qffitresultsbyindexevaluationfittools.h \
    ../base_classes/qffcstools.h \
    ../base_classes/qffitresultsbyindexevaluationfittools.h \
    ../base_classes/qffitchi2landscapedialog.h


SOURCES +=  qfpevalfcsfit.cpp \
            qffcsfitevaluation.cpp \
            qffcsfitevaluationeditor.cpp \
            dlgestimatefocalvolume.cpp \
            ../base_classes/qffitresultsevaluation.cpp \
            ../base_classes/qffitresultsbyindexevaluation.cpp \
            ../base_classes/qffitresultsevaluationeditorbase.cpp\
            ../base_classes/qffitresultsbyindexevaluationeditorbase.cpp \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.cpp \
    ../base_classes/qffitresultsbyindexevaluationfitthread.cpp \
    optionswidget.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffcstools.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp \
    ../base_classes/qffitchi2landscapedialog.cpp


FORMS =    estimatefocalvolumedlg.ui \
    optionswidget.ui \
    ../base_classes/qffitchi2landscapedialog.ui

RESOURCES += qfevalfcsfit.qrc \
    ../base_classes/qffitresultsevaluationeditorbase.qrc

TRANSLATIONS= ./translations/de.qfevalfcsfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

include(../../quickfit3_configqtwidgets.pri)




