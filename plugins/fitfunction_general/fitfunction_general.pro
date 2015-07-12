#
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#	
#	 
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

TARGET = fitfunction_general
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfgeneralfitfunctionsplugin.h \
           qffitfunctiongeneralsigmoid.h \
    qffitfunctiongeneralgaussianvar.h \
    qffitfunctiongenerallorentzian.h \
    qffitfunctiongeneralpowerlaw.h \
    qffitfunctiongeneralexp.h \
    qffitfunctiongeneraldblexp.h \
    qffitfunctiongeneralpolynom.h \
    qffitfunctiongeneralsine.h \
    qffitfunctiongenerallognormal.h \
    qffitfunctiongeneral2lognormal.h \
    qffitfunctiongeneralline.h \
    qffitfunctiongeneral3gaussianvar.h \
    qffitfunctiongeneral2gaussianvar.h \
    qffitfunctiongeneralmodhill5p.h \
    qffitfunctiongeneralhill.h \
    qffitfunctiongeneralcosine.h \
    qffitfunctiongeneralpolynomcentered.h \
    qffitfunctiongeneralgaussiandistvar.h \
    qffitfunctiongeneralskewgaussianvar.h \
    qffitfunctiongeneralsegmentedline.h \
    qffitfunctiongeneralsegmentedpowerlaw.h \
    qffitfunctiongeneralparabula.h \
    qffitfunctiongeneralpdfgaussianvar.h \
    qffitfunctiongeneralcdfgaussiandistvar.h \
    qffitfunctiongeneralgaussianbeamwidth.h \
    qffitfunctiongeneralgaussianbeamzintensity.h \
    qffitfunctiongeneralcdfcauchy.h \
    qffitfunctiongeneralpdfcauchy.h \
    qffitfunctiongeneralcdfloggaussian.h \
    qffitfunctiongeneralpdfloggaussian.h

SOURCES += qfgeneralfitfunctionsplugin.cpp\
           qffitfunctiongeneralsigmoid.cpp \
    qffitfunctiongeneralgaussianvar.cpp \
    qffitfunctiongenerallorentzian.cpp \
    qffitfunctiongeneralpowerlaw.cpp \
    qffitfunctiongeneralexp.cpp \
    qffitfunctiongeneraldblexp.cpp \
    qffitfunctiongeneralsine.cpp \
    qffitfunctiongeneralpolynom.cpp \
    qffitfunctiongenerallognormal.cpp \
    qffitfunctiongeneral2lognormal.cpp \
    qffitfunctiongeneralline.cpp \
    qffitfunctiongeneral3gaussianvar.cpp \
    qffitfunctiongeneral2gaussianvar.cpp \
    qffitfunctiongeneralmodhill5p.cpp \
    qffitfunctiongeneralhill.cpp \
    qffitfunctiongeneralcosine.cpp \
    qffitfunctiongeneralpolynomcentered.cpp \
    qffitfunctiongeneralgaussiandistvar.cpp \
    qffitfunctiongeneralskewgaussianvar.cpp \
    qffitfunctiongeneralsegmentedline.cpp \
    qffitfunctiongeneralsegmentedpowerlaw.cpp \
    qffitfunctiongeneralparabula.cpp \
    qffitfunctiongeneralpdfgaussianvar.cpp \
    qffitfunctiongeneralcdfgaussiandistvar.cpp \
    qffitfunctiongeneralgaussianbeamwidth.cpp \
    qffitfunctiongeneralgaussianbeamzintensity.cpp \
    qffitfunctiongeneralcdfcauchy.cpp \
    qffitfunctiongeneralpdfcauchy.cpp \
    qffitfunctiongeneralcdfloggaussian.cpp \
    qffitfunctiongeneralpdfloggaussian.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunction_general.ts


INCLUDEPATH += ../../lib/


include(../../quickfit3_configqt.pri)

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE

