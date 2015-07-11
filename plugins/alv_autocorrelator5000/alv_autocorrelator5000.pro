#
#    Copyright (c) 2008-2014 Benedikt Häusele, German Cancer Research Center
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
TARGET = alv_autocorrelator5000
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += \
     alv_autocorrelator5000.h \
    alvsma5edataout.h \
    ../interfaces/qfextensionacquisition.h \
    alvsma5edatain.h \
    alvsettingsdialog.h

SOURCES += \
    alv_autocorrelator5000.cpp \
    alvsma5edataout.cpp \
    alvsma5edatain.cpp \
    alvsettingsdialog.cpp

FORMS =

RESOURCES += \


TRANSLATIONS= ./translations/de.alv_autocorrelator5000.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../global_lib/ \
               ../../global_lib/qt/

Qinclude(../quickfit3_configqtwidgets.pri)


