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

TARGET = qfe_alexcontrol
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../../qf3plugins.pri)
include(../../libquickfitwidgets.pri)
include(../../extlibs/nidaqmx.pri)
include(../../extlibsb040/sdfflib.pri)
DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfe_alexcontrol.h \
           controlWidget.h \
           errormodel.h \
    alexceditsettingsfiles.h \
    optionswidget.h

SOURCES += qfe_alexcontrol.cpp \
           controlWidget.cpp \
           errormodel.cpp \
    alexceditsettingsfiles.cpp \
    optionswidget.cpp

FORMS += \
    controlWidget.ui \
    alexceditsettingsfiles.ui \
    optionswidget.ui

RESOURCES += qfe_alexcontrol.qrc

TRANSLATIONS= ./translations/de.qfe_alexcontrol.ts


include(../../quickfit3_configqtwidgets.pri)



