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



QT       += core gui

DEFINES +=QFLIB_TEST QFMATHPARSER_DEBUGFUNCTIONNAMES #QFMATHPARSER_BYTECODESTACK_ERRORCHECKING

TARGET = parser_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../qftools.cpp \
    ../../qfmathtools.cpp \
    ../../qfmathparser.cpp \
    ../../qfmathparserdefaultlib.cpp \
    ../../qfmathparsertools.cpp

HEADERS += \
    ../../qftools.h \
    ../../qfmathtools.h \
    ../../qfmathparser.h \
    ../../lib_imexport.h \
    ../../qfmathparserdefaultlib.h \
    ../../qfmathparsertools.h
