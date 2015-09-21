#
#    Copyright (c) 2015
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

TARGET = fit_nlopt
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
include(../../qf3plugins.pri)
include(../../extlibs/nlopt.pri)

DESTDIR = $${QFOUTPUT}/plugins/


# Input
HEADERS += qffitalgorithmnloptplugin.h \
    qffitalgorithmnlopttools.h \
    qffitalgorithmnloptbase.h \
    qffitalgorithmnloptnonderivconfigdialog.h \
    qffitalgorithmnloptalgorithms.h

SOURCES += qffitalgorithmnloptplugin.cpp \
    qffitalgorithmnlopttools.cpp \
    qffitalgorithmnloptbase.cpp \
    qffitalgorithmnloptnonderivconfigdialog.cpp

FORMS += \
    qffitalgorithmnloptnonderivconfigdialog.ui

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.fit_nlopt.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/


include(../../quickfit3_configqtwidgets.pri)
