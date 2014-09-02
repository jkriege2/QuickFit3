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


include(../../extlibs/cairo.pri)


QT       += core gui

TARGET = cairo4qt_test
#CONFIG   += console
#CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += cairo4qt_test.cpp \
    cairo4qt/src/brushpattern.cpp \
    cairo4qt/src/qcairopaintdevice.cpp \
    cairo4qt/src/qpaintengine_cairo.cpp

HEADERS += \
    cairo4qt/src/qcairopaintdevice.h \
    cairo4qt/src/qpaintengine_cairo_p.h \
    cairo4qt/src/qpaintengine_p.h \
    cairo4qt/src/qt_x11_p.h
