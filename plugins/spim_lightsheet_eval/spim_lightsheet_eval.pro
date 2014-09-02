#
#    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center
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

TARGET = spim_lightsheet_eval
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
include(../../libquickfitwidgets.pri)


# Input
HEADERS += qfspimlightsheetevaluation.h \
           qfspimlightsheetevaluation_item.h \
           qfspimlightsheetevaluation_editor.h \
    ../interfaces/qfrdrimagestackinterface.h \
    qfespimlightsheetevaluationcopyconfigdialog.h

SOURCES += qfspimlightsheetevaluation.cpp \
           qfspimlightsheetevaluation_item.cpp \
           qfspimlightsheetevaluation_editor.cpp \
    qfespimlightsheetevaluationcopyconfigdialog.cpp

FORMS = qfspimlightsheetevaluation_editor.ui \   
    qfespimlightsheetevaluationcopyconfigdialog.ui

RESOURCES += qfspimlightsheetevaluation.qrc

TRANSLATIONS= ./translations/de.spim_lightsheet_eval.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl


ASSETS_TESTDATA.files = ./examples/test_data/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/examples/$${TARGET}/test_data/
INSTALLS += ASSETS_TESTDATA


SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE


