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

TARGET = fit_gsl
DEFINES += TARGETNAME=$$TARGET

DEPENDPATH += ./
include(../plugins.pri)
include(../../extlibs/gsl.pri)

DESTDIR = $${QFOUTPUT}/plugins/


# Input
HEADERS += qffitalgorithmgslplugin.h \
		   qffitalgorithmgslsimplex.h \ 
    qffitalgorithmgsltools.h \
    qffitalgorithmgslsimplexconfigdialog.h \
    qffitalgorithmgslsimplexrand.h \
    qffitalgorithmgslderivconfigdialog.h \
    qffitalgorithmgslderivbase.h \
    qffitalgorithmgslbfgs2.h \
    qffitalgorithmgslsteepest_descent.h \
    qffitalgorithmgslconjugate_pr.h \
    qffitalgorithmgslconjugate_fr.h

SOURCES += qffitalgorithmgslplugin.cpp \
		   qffitalgorithmgslsimplex.cpp \
    qffitalgorithmgsltools.cpp \
    qffitalgorithmgslsimplexconfigdialog.cpp \
    qffitalgorithmgslsimplexrand.cpp \
    qffitalgorithmgslderivconfigdialog.cpp \
    qffitalgorithmgslderivbase.cpp \
    qffitalgorithmgslbfgs2.cpp


FORMS += \
    qffitalgorithmgslsimplexconfigdialog.ui \
    qffitalgorithmgslderivconfigdialog.ui

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.fit_gsl.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE
include(../../quickfit3_configqtwidgets.pri)
