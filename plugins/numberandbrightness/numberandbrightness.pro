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


TARGET = number_and_brightness
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrnumberandbrightness.h \
           qfrdrnumberandbrightness_data.h \
		   qfrdrnumberandbrightness_dataeditor.h \
    ../base_classes/qfrdrimagingfcstools.h \
    ../../../../../LIB/trunk/libtiff_tools.h \
    qfrdrnumberandbrightness_overvieweditor.h \
    ../base_classes/qfrdroverviewimagedisplay.h

SOURCES += qfrdrnumberandbrightness.cpp \
           qfrdrnumberandbrightness_data.cpp \
		   qfrdrnumberandbrightness_dataeditor.cpp \
    ../base_classes/qfrdrimagingfcstools.cpp \
    ../../../../../LIB/trunk/libtiff_tools.cpp \
    qfrdrnumberandbrightness_overvieweditor.cpp \
    ../base_classes/qfrdroverviewimagedisplay.cpp

FORMS = 

RESOURCES += numberandbrightness.qrc

TRANSLATIONS= ./translations/de.number_and_brightness.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core opengl
CONFIG += exceptions rtti stl


ASSETS_TESTDATA_FILES.files = ./examples/test_data/*.*
ASSETS_TESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/test_data

INSTALLS += ASSETS_TESTDATA_FILES

