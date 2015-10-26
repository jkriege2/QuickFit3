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


TARGET = number_and_brightness
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../../qf3plugins.pri)

include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrnumberandbrightness.h \
           qfrdrnumberandbrightness_data.h \
		   qfrdrnumberandbrightness_dataeditor.h \
    ../base_classes/qfrdrimagingfcstools.h \
    qfrdrnumberandbrightness_overvieweditor.h \
    ../base_classes/qfrdroverviewimagedisplay.h \
    ../base_classes/qfrdrimagemasktools.h \
    qfrdrnumberandbrightness_settingseditor.h \
    qfrdnnumberandbrightnessprocessingthread.h
#\
#    qfrdrnumberandbrightnessprocessingdialog.h \
#    qfrdrnumberandbrightnessprocessingjobthread.h

SOURCES += qfrdrnumberandbrightness.cpp \
           qfrdrnumberandbrightness_data.cpp \
		   qfrdrnumberandbrightness_dataeditor.cpp \
    ../base_classes/qfrdrimagingfcstools.cpp \
    qfrdrnumberandbrightness_overvieweditor.cpp \
    ../base_classes/qfrdroverviewimagedisplay.cpp \
    ../base_classes/qfrdrimagemasktools.cpp \
    qfrdrnumberandbrightness_settingseditor.cpp \
    qfrdnnumberandbrightnessprocessingthread.cpp
#\
#    qfrdrnumberandbrightnessprocessingdialog.cpp \
#    qfrdrnumberandbrightnessprocessingjobthread.cpp

FORMS = \ 
    qfrdrnumberandbrightness_settingseditor.ui
#\
#    qfrdrnumberandbrightnessprocessingdialog.ui

RESOURCES += numberandbrightness.qrc

TRANSLATIONS= ./translations/de.number_and_brightness.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

include(../../quickfit3_configqtwidgets.pri)


ASSETS_TESTDATA_FILES.files = ./examples/test_data/*.*
ASSETS_TESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/test_data

INSTALLS += ASSETS_TESTDATA_FILES

