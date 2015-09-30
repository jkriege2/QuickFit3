#
#    Copyright (c) 2012-2015 by Sebastian Isbaner
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

TARGET = qfe_alexeval
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../../qf3plugins.pri)
include(../../libquickfitwidgets.pri)
include(../../extlibs/gsl.pri)
include(../../extlibs/eigen.pri)


DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfe_alexeval.h

SOURCES += qfe_alexeval.cpp

FORMS +=

RESOURCES += qfe_alexeval.qrc

TRANSLATIONS += ./translations/de.qfe_alexeval.ts


HEADERS +=    parameters.h \
    readArrivalTimes.h \
    BurstSearch.h \
    analysis.h \
    photonswidget.h \
    burstsearchwidget.h \
    alexhistogramswidget.h \
    mainwindow.h \
    alexplots.h \
    alexdata.h \
    burst.h \
    compareburstsearch.h \
    DialogReconstructMarkerRecords.h \
    BurstView.h \
    BurstStatisticsView.h \
    dialogTable.h \
    alexevallog.h \
    GMMfit.h \
    FCSwidget.h \
    DialogGMMparams.h \
    FCSdata.h \
    FCScurve.h


SOURCES += readArrivalTimes.cpp \
    BurstSearch.cpp \
    analysis.cpp \
    parameters.cpp \
    photonswidget.cpp \
    burstsearchwidget.cpp \
    alexhistogramswidget.cpp \
    mainwindow.cpp \
    alexplots.cpp \
    alexdata.cpp \
    burst.cpp \
    compareburstsearch.cpp \
    DialogReconstructMarkerRecords.cpp \
    BurstView.cpp \
    BurstStatisticsView.cpp \
    dialogTable.cpp \
    alexevallog.cpp \
    GMMfit.cpp \
    FCSwidget.cpp \
    DialogGMMparams.cpp \
    FCSdata.cpp \
    FCScurve.cpp



FORMS += \
    photonswidget.ui \
    burstsearchwidget.ui \
    alexhistogramswidget.ui \
    DialogReconstructMarkerRecords.ui \
    BurstStatisticsView.ui \
    BurstView.ui \
    dialogTable.ui \
    FCSwidget.ui \
    DialogGMMparams.ui

include(../../quickfit3_configqtwidgets.pri)



