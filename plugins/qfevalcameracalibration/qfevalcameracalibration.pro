#
#    Copyright (c) 2014 
#	
#	 last modification: $LastChangedDate: 2014-06-24 16:05:58 +0200 (Di, 24 Jun 2014) $  (revision $Rev: 3289 $)
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

TARGET = eval_cameracalibration
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)
include(../../libquickfitwidgets.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfevalcameracalibration.h \
           qfevalcameracalibration_item.h \
           qfevalcameracalibration_editor.h

SOURCES += qfevalcameracalibration.cpp \
           qfevalcameracalibration_item.cpp \
           qfevalcameracalibration_editor.cpp

FORMS = qfevalcameracalibration_editor.ui   

RESOURCES += qfevalcameracalibration.qrc

TRANSLATIONS= ./translations/de.eval_cameracalibration.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl



