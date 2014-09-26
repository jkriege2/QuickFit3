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

TARGET = qfe_spectraviewer
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../libquickfitwidgets.pri)
include(../../extlibs/gsl.pri)
DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfespectraviewer.h \
    qfespectraviewerdialog.h \
    optionswidget.h \
    spectrummanager.h \
    qfespectraviewerlightsourceeditor.h \
    qfespectraviewerfluorophoreditor.h \
    qfespectraviewerfiltereditor.h \
    qfespectraviewerspilloverdialog.h \
    qfespectraviewerdetectoreditor.h \
    qfespectraviewerfretdialog.h

SOURCES += qfespectraviewer.cpp \
    qfespectraviewerdialog.cpp \
    optionswidget.cpp \
    spectrummanager.cpp \
    qfespectraviewerlightsourceeditor.cpp \
    qfespectraviewerfluorophoreditor.cpp \
    qfespectraviewerfiltereditor.cpp \
    qfespectraviewerspilloverdialog.cpp \
    qfespectraviewerdetectoreditor.cpp \
    qfespectraviewerfretdialog.cpp

FORMS = \
    qfespectraviewerdialog.ui \
    optionswidget.ui \
    qfespectraviewerlightsourceeditor.ui \
    qfespectraviewerfluorophoreditor.ui \
    qfespectraviewerfiltereditor.ui \
    qfespectraviewerspilloverdialog.ui \
    qfespectraviewerdetectoreditor.ui \
    qfespectraviewerfretdialog.ui

RESOURCES += qfespectraviewer.qrc \
    ../../lib/lib.qrc

TRANSLATIONS= ./translations/de.qfe_spectraviewer.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



