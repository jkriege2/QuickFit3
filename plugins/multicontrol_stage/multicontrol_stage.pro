#
#    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
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
TARGET = stage_mc2000
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += \
           ../interfaces/qfextensionlinearstage.h \
           ../../../../../LIB/trunk/jkserialconnection.h \
    multicontrol_stage.h \
    mcstagecontroller.h \
    ../b040_ffmcontrol/ffmmainlogfile.h \
    multicontrol_settingsdialog.h

SOURCES += \
           ../../../../../LIB/trunk/jkserialconnection.cpp \
    multicontrol_stage.cpp \
    mcstagecontroller.cpp \
    ../b040_ffmcontrol/ffmmainlogfile.cpp \
    multicontrol_settingsdialog.cpp

FORMS =

RESOURCES += \
    stage_mc2000.qrc

TRANSLATIONS= ./translations/de.stage_mc2000.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

include(../../quickfit3_configqtwidgets.pri)




