#
#    Copyright (c) 2014 
#	
#	 last modification: $LastChangedDate: 2015-01-21 11:37:05 +0100 (Mi, 21 Jan 2015) $  (revision $Rev: 3738 $)
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

TARGET = eval_colocalization
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)
include(../../libquickfitwidgets.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfevalcolocalization.h \
           qfevalcolocalization_item.h \
           qfevalcolocalization_editor.h

SOURCES += qfevalcolocalization.cpp \
           qfevalcolocalization_item.cpp \
           qfevalcolocalization_editor.cpp

FORMS = qfevalcolocalization_editor.ui   

RESOURCES += qfevalcolocalization.qrc

TRANSLATIONS= ./translations/de.eval_colocalization.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../global_lib/ \
               ../../global_lib/qt/

include(../../quickfit3_configqtwidgets.pri)



