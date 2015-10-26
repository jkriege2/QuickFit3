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

TARGET = qffcsfitfuncs
DEPENDPATH += ./

include(../../qf3plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
#DLLDESTDIR = $$DESTDIR



# Input
HEADERS += fcs_fitfuctions.h \
           qffitfunctionfcsmultidiffusion.h \
           qffitfunctionfcsadiff.h \
           qffitfunctionfcsdiff.h \
           qffitfunctionfcssimplediff.h \
    qffitfunctionfcsflow.h \
    qffitfunctionfcsrot.h \
    qffitfunctionfcsoscillation.h \
    qffitfunctionfcsdiffD.h \
    qffitfunctionfcsflowv.h \
    qffitfunctioncsfcs.h \
    qffitfunctionfcsexp.h \
    qffitfunctionfcssimpleexp.h \
    qffitfunctionfcspowerlaw.h \
    qffitfunctionfcsdiffDAfterpulse.h \
    qffitfunctionfcsdiffDcAfterpulse.h \
    qffitfunctionfcsdiff_relbright.h \
    qffitfunctionfcsadiff_relbright.h \
    qffitfunctionfcsdiff2d_relbright.h \
    qffitfunctionfcsdiffstretchpp.h \
    qffitfunctionfcsadiff2d.h \
    qffitfunctionfcsadiff2d_relbright.h \
    qffitfunctionfcsdiff2d.h \
    qffitfunctionfcsdiff2dstretchpp.h

SOURCES += fcs_fitfuctions.cpp\
           qffitfunctionfcsmultidiffusion.cpp \
           qffitfunctionfcsadiff.cpp \
           qffitfunctionfcsdiff.cpp \
           qffitfunctionfcssimplediff.cpp \
    qffitfunctionfcsflow.cpp \
    qffitfunctionfcsrot.cpp \
    qffitfunctionfcsoscillation.cpp \
    qffitfunctionfcsdiffD.cpp \
    qffitfunctionfcsflowv.cpp \
    qffitfunctioncsfcs.cpp \
    qffitfunctionfcsexp.cpp \
    qffitfunctionfcssimpleexp.cpp \
    qffitfunctionfcspowerlaw.cpp \
    qffitfunctionfcsdiffDAfterpulse.cpp \
    qffitfunctionfcsdiffDcAfterpulse.cpp \
    qffitfunctionfcsdiff_relbright.cpp \
    qffitfunctionfcsadiff_relbright.cpp \
    qffitfunctionfcsdiff2d_relbright.cpp \
    qffitfunctionfcsdiffstretchpp.cpp \
    qffitfunctionfcsadiff2d.cpp \
    qffitfunctionfcsadiff2d_relbright.cpp \
    qffitfunctionfcsdiff2d.cpp \
    qffitfunctionfcsdiff2dstretchpp.cpp



FORMS +=

RESOURCES +=

TRANSLATIONS += ./translations/de.qffcsfitfuncs.ts


INCLUDEPATH += ../../lib/


include(../../quickfit3_configqt.pri)


#SRC_DISTRIBUTED.files = $$HEADERS \
#                        $$SOURCES \
#                        $$FORMS \
#                        $$RESOURCES \
#                        $$TRANSLATIONS \
#                        $$GPL3LICENSE





