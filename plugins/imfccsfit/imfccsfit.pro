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

TARGET = imfccs_fit
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfimfccsfitevaluation.h \
           qfimfccsfitevaluation_item.h \
           qfimfccsfitevaluation_editor.h \
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
    qfimfccsparameterinputtable.h \
    qfimfccsparameterinputdelegate.h \
    qfimfccsrelativeccfdialog.h \
    qfimfccsrelativeintensitydialog.h \
    qfimfccsfilesetsmodel.h \
    qfimfccsfitchi2landscapedialog.h \
    ../interfaces/qffitfunctionconfigforglobalfitinterface.h \
    ../base_classes/qffitresultsbyindexevaluationfitthread.h \
    qfimfccsrelativeccfcrosstalkdialog.h \
    qfimfccsamplitudefitdialog.h \
    ../base_classes/qffitfunctionconfigforglobalfittools.h \
    ../base_classes/dlgfccsresetsetparameter.h

SOURCES += qfimfccsfitevaluation.cpp \
           qfimfccsfitevaluation_item.cpp \
           qfimfccsfitevaluation_editor.cpp \
    ../base_classes/qffitresultsbyindexasvectorevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluation.cpp \
    ../base_classes/qffitresultsevaluation.cpp \
    ../base_classes/qffitresultsbyindexevaluationfittools.cpp \
    ../base_classes/qffcsweightingtools.cpp \
    ../base_classes/qffcstools.cpp \
    optionswidget.cpp \
    qfimfccsparameterinputtable.cpp \
    qfimfccsparameterinputdelegate.cpp \
    qfimfccsrelativeccfdialog.cpp \
    qfimfccsrelativeintensitydialog.cpp \
    qfimfccsfilesetsmodel.cpp \
    qfimfccsfitchi2landscapedialog.cpp \
    ../base_classes/qffitresultsbyindexevaluationfitthread.cpp \
    qfimfccsrelativeccfcrosstalkdialog.cpp \
    qfimfccsamplitudefitdialog.cpp \
    ../base_classes/qffitfunctionconfigforglobalfittools.cpp \
    ../base_classes/dlgfccsresetsetparameter.cpp

FORMS = qfimfccsfitevaluation_editor.ui \   
    optionswidget.ui \
    qfimfccsrelativeccfdialog.ui \
    qfimfccsrelativeintensitydialog.ui \
    qfimfccsfitchi2landscapedialog.ui \
    qfimfccsrelativeccfcrosstalkdialog.ui  \
    qfimfccsamplitudefitdialog.ui\
    ../base_classes/dlgfccsresetsetparameter.ui




RESOURCES += qfimfccsfitevaluation.qrc  \
    ../../lib/lib.qrc

TRANSLATIONS= ./translations/de.imfccs_fit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

include(../../quickfit3_configqtwidgets.pri)


ASSETS_RESULTS.files = ./examples/results/*.*
ASSETS_RESULTS.path = $${QFOUTPUT}/examples/$${TARGET}/results/

INSTALLS += ASSETS_RESULTS

