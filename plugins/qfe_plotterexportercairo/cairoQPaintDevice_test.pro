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


#include(../../extlibs/cairo.pri)

INCLUDEPATH +=  cairo pixman-1
LIBS +=  -lcairo -lpixman-1 -lz -lpng
DEFINES += QF_HAS_LIBCAIRO QF_HAS_LIBPIXMAN
win32:LIBS += -lgdi32

QT       += core gui

TARGET = cairoQPaintDevice_test
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += cairoQPaintDevice_test.cpp \
    qcairopaintdevice.cpp \
    qcairopaintengine.cpp

HEADERS += \
    qcairopaintdevice.h \ 
    qcairopaintengine.h
    
