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

TARGET = tcspcimporter
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfetcspcimporter.h \
    qfetcspcimporterthreadprogress.h \
    qfetcspcimporterjobthread.h \
    qfetcspcimporterdialog.h \
    ../base_classes/qf3correlationdataformattool.h \
    ../base_classes/tttrtools.h \
    qfetcspcimporterlifetimeview.h

SOURCES += qfetcspcimporter.cpp \
    qfetcspcimporterthreadprogress.cpp \
    qfetcspcimporterjobthread.cpp \
    qfetcspcimporterdialog.cpp \
    ../base_classes/qf3correlationdataformattool.cpp \
    ../base_classes/tttrtools.cpp \
    qfetcspcimporterlifetimeview.cpp

FORMS = \
    qfetcspcimporterthreadprogress.ui \
    qfetcspcimporterdialog.ui \
    qfetcspcimporterlifetimeview.ui

RESOURCES += qfetcspcimporter.qrc \
    ../../lib/lib.qrc

TRANSLATIONS= ./translations/de.tcspcimporter.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

include(../../quickfit3_configqtwidgets.pri)

ASSETS_TESTDATA.files = ./assets/testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/testdata/

INSTALLS += ASSETS_TESTDATA






