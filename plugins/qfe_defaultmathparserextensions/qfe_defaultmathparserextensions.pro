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

TARGET = qfe_defaultmathparserextensions
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfedefaultmathparserextensions.h \
    qfedefaultmathparserextensionfunctions.h \
    ../../../../../LIB/trunk/statistics_tools.h

SOURCES += qfedefaultmathparserextensions.cpp \
    qfedefaultmathparserextensionfunctions.cpp \
    ../../../../../LIB/trunk/statistics_tools.cpp

FORMS =

RESOURCES += qfedefaultmathparserextensions.qrc

TRANSLATIONS= ./translations/de.qfe_defaultmathparserextensions.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

HELPREF_FILES.files = ./help/parserreference/*.*
HELPREF_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/

HELPREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPREFPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/pic/

INSTALLS +=  HELPREF_FILES HELPREFPIC_FILES


QT += gui xml svg
CONFIG += exceptions rtti stl



