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

TARGET = qfe_plotterexporterlatex
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfeplotterexporterlatex.h \
    jkqtplatexengineadapter.h \
    QTeXEngine/src/QTeXEngine.h

SOURCES += qfeplotterexporterlatex.cpp \
    jkqtplatexengineadapter.cpp \
    QTeXEngine/src/QTeXPaintEngine.cpp \
    QTeXEngine/src/QTeXPaintDevice.cpp

win32 {
  DEFINES += HAS_EMF_ENGINE
  SOURCES += EmfEngine/src/EmfEngine.cpp \
             EmfEngine/src/EmfPaintDevice.cpp \
             jkqtpemfengineadapter.cpp

  HEADERS += EmfEngine/src/EmfEngine.h \
             jkqtpemfengineadapter.h
}

FORMS =

RESOURCES += qfeplotterexporterlatex.qrc

TRANSLATIONS= ./translations/de.qfe_plotterexporterlatex.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl




SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE \
                        QTeXEngine-0.3-opensource.zip \
                        EmfEngine-opensource-0.8.zip
