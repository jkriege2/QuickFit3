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

include(../../../../../SPIM/trunk/software/processing_chain/processing_chain.pri)

QMAKE_CXXFLAGS += -ggdb

TEMPLATE = lib
CONFIG += plugin

TARGET = cam_rh2v2
DEPENDPATH += ./

LIBS += -lusb

include(../plugins.pri)
include(../../extlibs/usb1.0.pri)

DESTDIR = $$QFOUTPUT/plugins


DEFINES += TARGETNAME=$$TARGET

include(../../libquickfitwidgets.pri)

# Input
HEADERS += cam_rh2v2.h \
           ../interfaces/qfextensioncamera.h \
    qfradhard2flashtool.h \
    ../base_classes/radhard2flash.h \
    cam_rh2v2_cfgdlg.h \
    cam_rh2v2_cordlg.h

SOURCES += cam_rh2v2.cpp \
    qfradhard2flashtool.cpp \
    ../base_classes/radhard2flash.cpp \
    cam_rh2v2_cfgdlg.cpp \
    cam_rh2v2_cordlg.cpp

FORMS = \
    qfradhard2flashtool.ui \
    cam_rh2v2_cfgdlg.ui \
    cam_rh2v2_cordlg.ui

RESOURCES += cam_rh2v2.qrc

TRANSLATIONS= ./translations/de.cam_rh2v2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

include(../../quickfit3_configqtwidgets.pri)

OTHER_FILES += \
    cam_rh2v2.cor.ini \
    cam_rh2v2.raw.ini
