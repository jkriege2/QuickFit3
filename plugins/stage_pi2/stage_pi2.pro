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
TARGET = stage_pi863_2
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += stage_pi2.h \
           ../interfaces/qfextensionlinearstage.h \
           ../../../../../LIB/trunk/jkserialconnection.h \
    pimercury863calibrationdialog.h \
    ../base_classes/qf3comportmanager.h \
    qfextensionlinearstagepi2protocolhandler.h

SOURCES += stage_pi2.cpp \
           ../../../../../LIB/trunk/jkserialconnection.cpp \
    pimercury863calibrationdialog.cpp \
    ../base_classes/qf3comportmanager.cpp \
    qfextensionlinearstagepi2protocolhandler.cpp

FORMS =

RESOURCES += stage_pi2.qrc

TRANSLATIONS= ./translations/de.stage_pi863_2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg




