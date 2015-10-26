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



QT       += core network gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

DEFINES += QFLIB_LIBRARY
CONFIG   += console
CONFIG   -= app_bundle

TARGET = tools_test

TEMPLATE = app

include(../../../extlibsb040/tinymat.pri)

SOURCES += main.cpp \
    ../../qftools.cpp \
    ../../programoptions.cpp \
    ../../matlabtools.cpp \
    ../../datatools.cpp \
    ../../csvtools.cpp \
    ../../binarydatatools.cpp \
    ../../qfdlgcsvparameters.cpp \
    ../../qfhtmlhelptools.cpp \
    ../../qfpluginservices.cpp \
    threadtest.cpp \
    ../../qconfigcomboboxwidget.cpp \
    ../../qvisiblehandlesplitter.cpp \
    ../../qfdoubleedit.cpp \
    ../../qffitfunctionbase.cpp \
    ../../qftablemodel.cpp \
    ../../qfversion.cpp

HEADERS += \
    ../../qftools.h \
    ../../qfpreprocessormagic_private.h \
    ../../qfpreprocessormagic.h \
    ../../programoptions.h \
    ../../matlabtools.h \
    ../../datatools.h \
    ../../csvtools.h \
    ../../binarydatatools.h \
    ../../qfdlgcsvparameters.h \
    ../../qfhtmlhelptools.h \
    threadtest.h \
    ../../qconfigcomboboxwidget.h \
    ../../qvisiblehandlesplitter.h \
    ../../qfdoubleedit.h \
    ../../qffitfunctionbase.h \
    ../../qftablemodel.h \
    ../../qfversion.h

FORMS += \
    ../../qfdlg_csvparameters.ui

INCLUDEPATH += ../../
