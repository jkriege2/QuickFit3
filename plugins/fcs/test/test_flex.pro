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



QT       += core gui network


TARGET = test_flex
CONFIG   += console

TEMPLATE = app

DEFINES +=  QFLIB_LIBRARY

INCLUDEPATH += ../../../lib/ ../../../../../../LIB/trunk/


SOURCES += test_flex.cpp \
    ../../../lib/qfmathtools.cpp \
    ../../../lib/qftools.cpp \
    ../flex_sin_tools.cpp \
    ../../../../../../LIB/trunk/tools.cpp \
    ../../../lib/datatools.cpp \
    ../../../lib/csvtools.cpp \
    ../../../lib/matlabtools.cpp \
    ../../../lib/programoptions.cpp

HEADERS += \
    ../../../lib/qfmathtools.h \
    ../../../lib/qftools.h \
    ../flex_sin_tools.h \
    ../../../../../../LIB/trunk/tools.h \
    ../../../lib/datatools.h \
    ../../../lib/csvtools.h \
    ../../../lib/matlabtools.h \
    ../../../lib/programoptions.h
