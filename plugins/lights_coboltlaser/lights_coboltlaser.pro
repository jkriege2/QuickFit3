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

TARGET = lights_coboltlaser
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += \
    ../base_classes/qf3comportmanager.h \
    ../interfaces/qfextensionlightsource.h \
    ../../../../../LIB/trunk/jkserialconnection.h \
    qfextensioncoboltlaser.h \
    qfcoboltlaserprotocolhandler.h \
    ../base_classes/qfsimpleserialprotocolhandler.h \
    ../interfaces/qfextensionmeasurementdevice.h


SOURCES += \
    ../base_classes/qf3comportmanager.cpp \
    ../../../../../LIB/trunk/jkserialconnection.cpp \
    qfextensioncoboltlaser.cpp \
    qfcoboltlaserprotocolhandler.cpp \
    ../base_classes/qfsimpleserialprotocolhandler.cpp

FORMS =

RESOURCES += lights_coboltlaser.qrc

TRANSLATIONS= ./translations/de.lights_coboltlaser.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



