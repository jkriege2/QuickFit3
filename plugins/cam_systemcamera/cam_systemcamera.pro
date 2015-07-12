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

TARGET = cam_systemcam
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/cimg.pri)

DESTDIR = $${QFOUTPUT}/plugins/


# Input
HEADERS += qfecamsystemcamera.h \
           ../interfaces/qfextensioncamera.h \
        videocapture.h \
    syscamconfigdialog.h

SOURCES += qfecamsystemcamera.cpp \
    syscamconfigdialog.cpp


unix {
  LIBS +=
  SOURCES += videocapture_v4l2.cpp
}

win32 {
  LIBS += -lgdi32 -luser32 -lkernel32 -lvfw32
  SOURCES += videocapture_vfw.cpp
}


FORMS = \
    syscamconfigdialog.ui

RESOURCES += qfecamsystemcamera.qrc

TRANSLATIONS= ./translations/de.cam_systemcam.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

include(../../quickfit3_configqtwidgets.pri)



