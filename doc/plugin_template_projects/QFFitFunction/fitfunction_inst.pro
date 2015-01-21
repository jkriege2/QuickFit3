#
#    Copyright (c) 2014 
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

TARGET = target_id
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += fitfunction_inst.h \
           fitfunction_inst_f1.h

SOURCES += fitfunction_inst.cpp\
           fitfunction_inst_f1.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.target_id.ts


INCLUDEPATH += ../../lib/


include(../../quickfit3_configqt.pri)
