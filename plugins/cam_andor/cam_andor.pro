#
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
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

TARGET = cam_andor
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/andor.pri)

DESTDIR = $$QFOUTPUT/plugins


DEFINES += TARGETNAME=$$TARGET



include(../../libquickfitwidgets.pri)
include(../../extlibsb040/tinytiff.pri)
# Input
HEADERS += cam_andor.h \
           camandoracquisitionthread.h \
           ../interfaces/qfextensioncamera.h \
           andorglobalcamerasettingswidget.h \
           andorsettingsdialog.h \
           andortools.h \
           ../interfaces/qfextensionshutter.h \
    ../interfaces/qfextensionmeasurementdevice.h

SOURCES += cam_andor.cpp \
           camandoracquisitionthread.cpp \
           andorglobalcamerasettingswidget.cpp \
           andorsettingsdialog.cpp \
           andortools.cpp

FORMS = \
    andorglobalcamerasettingswidget.ui \
    andorsettingsdialog.ui

RESOURCES += cam_andor.qrc

TRANSLATIONS= ./translations/de.cam_andor.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/





include(../../quickfit3_configqtwidgets.pri)
