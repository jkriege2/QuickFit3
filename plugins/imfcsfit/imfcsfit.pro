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

#
#    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#
#	last modification: $LastChangedDate$  (revision $Rev$)
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


TEMPLATE = lib
CONFIG += plugin

TARGET = imfcsfit
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)
include(../../extlibs/cimg.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfpevalimfcsfit.h \
           qfimfcsfitevaluation.h \
           qfimfcsfitevaluationeditor.h \
           ../interfaces/qfrdrfcsdatainterface.h \
		   ../interfaces/qfrdrcountratesinterface.h \
    ../base_classes/qffitresultsevaluation.h \
    ../base_classes/qffitresultsbyindexevaluation.h \
    ../base_classes/qffitresultsevaluationeditorbase.h \
    ../base_classes/qffitresultsbyindexevaluationeditorbase.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../base_classes/qffitresultsbyindexasvectorevaluation.h \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.h \
    imfcscalibrationdialog.h \
    ../base_classes/qffitresultsbyindexevaluationfitthread.h \
    optionswidget.h \
    ../base_classes/qffcsweightingtools.h \
    ../base_classes/qffitresultsbyindexevaluationfittools.h \
    ../base_classes/qffcstools.h \
    qfimfcssetparamfromfiledialog.h \
    imfcscalibrationwizard.h \
    ../interfaces/qfrdrimfcscorrelatorremote.h \
    ../base_classes/qffcsfitchi2landscapedialog.h\
    ../base_classes/qfimagetransform.h \
    ../../../../../LIB/trunk/libtiff_tools.h


SOURCES += qfpevalimfcsfit.cpp \
           qfimfcsfitevaluation.cpp \
           qfimfcsfitevaluationeditor.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp \
    ../base_classes/qffitresultsevaluationeditorbase.cpp\
    ../base_classes/qffitresultsbyindexevaluationeditorbase.cpp \
    ../base_classes/qffitresultsevaluation.cpp \
    ../base_classes/qffitresultsbyindexasvectorevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluationeditorwithwidgets.cpp \
    imfcscalibrationdialog.cpp \
    ../base_classes/qffitresultsbyindexevaluationfitthread.cpp \
    optionswidget.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp \
    ../base_classes/qffcstools.cpp \
    qfimfcssetparamfromfiledialog.cpp \
    imfcscalibrationwizard.cpp \
    ../base_classes/qffcsfitchi2landscapedialog.cpp\
    ../base_classes/qfimagetransform.cpp \
    ../../../../../LIB/trunk/libtiff_tools.cpp



FORMS = \    
    imfcscalibrationdialog.ui \
    optionswidget.ui \
    qfimfcssetparamfromfiledialog.ui \
    imfcscalibrationwizard.ui \
    ../base_classes/qffcsfitchi2landscapedialog.ui

RESOURCES += qfevalimfcsfit.qrc \
    ../base_classes/qffitresultsevaluationeditorbase.qrc

TRANSLATIONS= ./translations/de.qfevalimfcsfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE

QT += gui xml svg core opengl
CONFIG += exceptions rtti stl
