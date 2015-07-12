#
#    Copyright (c) 2008-2014 Benedikt Häusele, Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center
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

TARGET = b040_ffmcontrol
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/
include(../../libquickfitwidgets.pri)



# Input
HEADERS += qfeb040ffmcontrol.h \
    ffmmainwindow.h \
    ffmexpcontrolwidget.h \
    ffmimagewidget.h \
    ffmexpdescriptionwidget.h \
    ../../global_lib/qfserialconnection.h \




SOURCES += qfeb040ffmcontrol.cpp \
    ffmmainwindow.cpp \
    ffmexpcontrolwidget.cpp \
    ffmimagewidget.cpp \
    ffmexpdescriptionwidget.cpp \
    ../../global_lib/qfserialconnection.cpp \



FORMS =

RESOURCES += qfeb040ffmcontrol.qrc

TRANSLATIONS= ./translations/de.b040_ffmcontrol.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../global_lib/ \
               ../../global_lib/qt/ \


include(../quickfit3_configqtwidgets.pri)




