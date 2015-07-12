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
TARGET = meas_b040resheater
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += meas_b040resheater.h \
           ../../global_lib/qfserialconnection.h \
    ../base_classes/qf3simpleb040serialprotocolhandler.h \
    ../base_classes/qf3comportmanager.h \
    ../interfaces/qfextensionmeasurementdevice.h

SOURCES += meas_b040resheater.cpp \
           ../../global_lib/qfserialconnection.cpp \
    ../base_classes/qf3simpleb040serialprotocolhandler.cpp \
    ../base_classes/qf3comportmanager.cpp

FORMS =

RESOURCES += meas_b040resheater.qrc

TRANSLATIONS= ./translations/de.meas_b040resheater.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../global_lib/ \
               ../../global_lib/qt/

include(../../quickfit3_configqtwidgets.pri)






