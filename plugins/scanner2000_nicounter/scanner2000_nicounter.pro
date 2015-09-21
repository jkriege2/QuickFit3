#
#    Copyright (c) 2008-2014 Benedikt Häusele, German Cancer Research Center
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
TARGET = scanner2000_nicounter
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../../qf3plugins.pri)
include(../../extlibs/nidaqmx.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

HEADERS += \
           scanner2000nicounter.h \
           scancontroller.h \
           ../../extlibs/NIDAQmx/NIDAQmx.h \
    nidaq6602.h


SOURCES += \
           scanner2000nicounter.cpp \
           scancontroller.cpp \
    nidaq6602.cpp

FORMS =

RESOURCES += \


TRANSLATIONS= ./translations/de.stage_mc2000.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

include(../../quickfit3_configqtwidgets.pri)
