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

TARGET = cam_server
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)
include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/


# Input
HEADERS += qfecamserver.h \
           ../interfaces/qfextensioncamera.h \
    ../interfaces/qfextensionmeasurementdevice.h

SOURCES += qfecamserver.cpp

FORMS =

RESOURCES += qfecamserver.qrc

TRANSLATIONS= ./translations/de.cam_server.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../global_lib/ \
               ../../global_lib/qt/

include(../../quickfit3_configqtwidgets.pri)


EXAMPLESTINYTIFF_FILES.files = ../../global_lib/tinytiff*.*
EXAMPLESTINYTIFF_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/
EXAMPLESHIGHRESTIMER_FILES.files = ../../global_lib/highrestimer.*
EXAMPLESHIGHRESTIMER_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/


INSTALLS += EXAMPLESTINYTIFF_FILES EXAMPLESHIGHRESTIMER_FILES


