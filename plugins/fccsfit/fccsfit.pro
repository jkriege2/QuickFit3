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

TARGET = fccs_fit
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../../qf3plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qffccsfitevaluation.h \
           qffccsfitevaluation_item.h \
           qffccsfitevaluation_editor.h \
    ../base_classes/qffitresultsbyindexasvectorevaluation.h \
    ../interfaces/qfrdrsimplecountrateinterface.h \
    ../interfaces/qfrdrfcsdatainterface.h \
    ../interfaces/qfrdrcountratesinterface.h \
    ../base_classes/qffitresultsbyindexevaluation.h \
    ../base_classes/qffitresultsevaluation.h \
    ../base_classes/qffitresultsbyindexevaluationfittools.h \
    ../base_classes/qffcsweightingtools.h \
    ../base_classes/qffcstools.h \
    optionswidget.h \
    qffccsparameterinputtable.h \
    qffccsparameterinputdelegate.h \
    qffccsfilesetsmodel.h \
    ../base_classes/qfrdrrunselector.h \
    dlgfccsfitallconfiguration.h \
    qffccsrelativeccfcrosstalkdialog.h \
    ../base_classes/qffitfunctionconfigforglobalfittools.h \
    ../base_classes/dlgfccsresetsetparameter.h

SOURCES += qffccsfitevaluation.cpp \
           qffccsfitevaluation_item.cpp \
           qffccsfitevaluation_editor.cpp \
    ../base_classes/qffitresultsbyindexasvectorevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp \
    ../base_classes/qffitresultsevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffcstools.cpp \
    optionswidget.cpp \
    qffccsparameterinputtable.cpp \
    qffccsparameterinputdelegate.cpp \
    qffccsfilesetsmodel.cpp \
    ../base_classes/qfrdrrunselector.cpp \
    dlgfccsfitallconfiguration.cpp \
    qffccsrelativeccfcrosstalkdialog.cpp \
    ../base_classes/qffitfunctionconfigforglobalfittools.cpp\
    ../base_classes/dlgfccsresetsetparameter.cpp

FORMS = qffccsfitevaluation_editor.ui \   
    optionswidget.ui \
    dlgfccsfitallconfiguration.ui \
    qffccsrelativeccfcrosstalkdialog.ui \
    ../base_classes/dlgfccsresetsetparameter.ui

SOURCES +=

FORMS +=

HEADERS +=


RESOURCES += qffccsfitevaluation.qrc

TRANSLATIONS= ./translations/de.fccs_fit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

include(../../quickfit3_configqtwidgets.pri)


ASSETS_RESULTS.files = ./examples/results/*.*
ASSETS_RESULTS.path = $${QFOUTPUT}/examples/$${TARGET}/results/

INSTALLS += ASSETS_RESULTS

