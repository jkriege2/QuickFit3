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

TARGET = fitfunctions_lightsheet
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qffitfunctionslightsheet.h \
           qffitfunctionlightsheetgaussian.h \
           qffitfunctionlightsheetslit.h \
           qffitfunctionlightsheetgaussianvar.h \
    ../../../../../LIB/trunk/statistics_tools.h \
    qffitfunctionlightsheetgaussianfwhm.h

SOURCES += qffitfunctionslightsheet.cpp\
           qffitfunctionlightsheetgaussian.cpp \
           qffitfunctionlightsheetslit.cpp \
           qffitfunctionlightsheetgaussianvar.cpp \
    ../../../../../LIB/trunk/statistics_tools.cpp \
    qffitfunctionlightsheetgaussianfwhm.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_lightsheet.ts


INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE


