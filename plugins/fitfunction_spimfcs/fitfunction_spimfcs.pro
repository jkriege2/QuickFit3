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

TARGET = fitfunctions_spimfcs
DEPENDPATH += ./

include(../plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionsspimfcs.h \
           qffitfunctionsspimfcsdiff.h \
    qffitfunctionsspimfcsadiff.h \
    qffitfunctionsspimfcsdiffe2.h \
    qffitfunctionsspimfccsdiffflow.h \
    qffitfunctionsspimfccsdiff2color.h \
    qffitfunctionsspimfcsdiffc.h \
    qffitfunctionsspimfcsdiffe2_newveff.h \
    qffitfunctionsspimfcsdiffce2.h \
    qffitfunctionsspimfcsadiffe2.h \
    ../base_classes/imfcstools.h \
    qffitfunctionsspimfcsadiffce2.h \
    qffitfunctionsspimfccsfwdiff2colorccf.h \
    qffitfunctionsspimfccsfwdiff2coloracfr.h \
    qffitfunctionsspimfccsfwdiff2coloracfg.h \
    qffitfunctionsspimfccsfwfactordiff2colorccf.h \
    qffitfunctionsspimfccsfwfactordiff2coloracfr.h \
    qffitfunctionsspimfccsfwfactordiff2coloracfg.h \
    qffitfunctionsspimfccsfwadiff2colorccf.h \
    qffitfunctionsspimfccsfwadiff2coloracfr.h \
    qffitfunctionsspimfccsfwadiff2coloracfg.h \
    qffitfunctionsspimfccsfw2cdiff2colorccf.h \
    qffitfunctionsspimfccsfw2cdiff2coloracfr.h \
    qffitfunctionsspimfccsfw2cdiff2coloracfg.h \
    qffitfunctionsspimfccsdiff.h \
    ../interfaces/qffitfunctionconfigforglobalfitinterface.h \
    qffitfunctionsspimfccsfw2csepdiff2colorccf.h \
    qffitfunctionsspimfccsfw2csepdiff2coloracfr.h \
    qffitfunctionsspimfccsfw2csepdiff2coloracfg.h \
    qffitfunctionsspimfccsfw2csep2ddiffxy2colorccf.h \
    qffitfunctionsspimfccsfw2csep2ddiffxy2coloracfr.h \
    qffitfunctionsspimfccsfw2csep2ddiffxy2coloracfg.h \
    qffitfunctionsspimfccsfw2csep2ddiffxz2colorccf.h \
    qffitfunctionsspimfccsfw2csep2ddiffxz2coloracfr.h \
    qffitfunctionsspimfccsfw2csep2ddiffxz2coloracfg.h \
    qffitfunctionsspimfccsfwsepdiff2colorccf.h \
    qffitfunctionsspimfccsfwsepdiff2coloracfr.h \
    qffitfunctionsspimfccsfwsepdiff2coloracfg.h \
    qffitfunctionsspimfccsdiffflowce2_newVeff.h \
qffitfunctionsspimfcsdiffe2_newveff_relbright.h \
    qffitfunctionsspimfccsdiffflowce2_newVeff_2.h \
    qffitfunctionsspimfccsdiffce2_newVeff.h \
    qffitfunctionsspimfcsdiffflowce2_newVeff.h \
    qffitfunctionsspimfccsfw2csepadiff2coloracfg.h \
    qffitfunctionsspimfccsfw2csepadiff2coloracfr.h \
    qffitfunctionsspimfccsfw2csepadiff2colorccf.h


SOURCES += qfpfitfunctionsspimfcs.cpp\
           qffitfunctionsspimfcsdiff.cpp \
    qffitfunctionsspimfcsadiff.cpp \
    qffitfunctionsspimfcsdiffe2.cpp \
    qffitfunctionsspimfccsdiffflow.cpp \
    qffitfunctionsspimfccsdiff2color.cpp \
    qffitfunctionsspimfcsdiffc.cpp \
    qffitfunctionsspimfcsdiffe2_newveff.cpp \
    qffitfunctionsspimfcsdiffce2.cpp \
    qffitfunctionsspimfcsadiffe2.cpp \
    ../base_classes/imfcstools.cpp \
    qffitfunctionsspimfcsadiffce2.cpp \
    qffitfunctionsspimfccsfwdiff2coloracfg.cpp \
    qffitfunctionsspimfccsfwdiff2colorccf.cpp \
    qffitfunctionsspimfccsfwfactordiff2coloracfr.cpp \
    qffitfunctionsspimfccsfwfactordiff2coloracfg.cpp \
    qffitfunctionsspimfccsfwfactordiff2colorccf.cpp \
    qffitfunctionsspimfccsfwdiff2coloracfr.cpp \
    qffitfunctionsspimfccsfwadiff2coloracfg.cpp \
    qffitfunctionsspimfccsfwadiff2colorccf.cpp \
    qffitfunctionsspimfccsfwadiff2coloracfr.cpp \
    qffitfunctionsspimfccsfw2cdiff2colorccf.cpp \
    qffitfunctionsspimfccsfw2cdiff2coloracfr.cpp \
    qffitfunctionsspimfccsfw2cdiff2coloracfg.cpp \
    qffitfunctionsspimfccsdiff.cpp \
    qffitfunctionsspimfccsfw2csepdiff2colorccf.cpp \
    qffitfunctionsspimfccsfw2csepdiff2coloracfr.cpp \
    qffitfunctionsspimfccsfw2csepdiff2coloracfg.cpp \
    qffitfunctionsspimfccsfw2csep2ddiffxy2colorccf.cpp \
    qffitfunctionsspimfccsfw2csep2ddiffxy2coloracfr.cpp \
    qffitfunctionsspimfccsfw2csep2ddiffxy2coloracfg.cpp \
    qffitfunctionsspimfccsfw2csep2ddiffxz2colorccf.cpp \
    qffitfunctionsspimfccsfw2csep2ddiffxz2coloracfr.cpp \
    qffitfunctionsspimfccsfw2csep2ddiffxz2coloracfg.cpp \
    qffitfunctionsspimfccsfwsepdiff2colorccf.cpp \
    qffitfunctionsspimfccsfwsepdiff2coloracfr.cpp \
    qffitfunctionsspimfccsfwsepdiff2coloracfg.cpp \
    qffitfunctionsspimfccsdiffflowce2_newVeff.cpp \
qffitfunctionsspimfcsdiffe2_newveff_relbright.cpp \
    qffitfunctionsspimfccsdiffflowce2_newVeff_2.cpp \
    qffitfunctionsspimfccsdiffce2_newVeff.cpp \
    qffitfunctionsspimfcsdiffflowce2_newVeff.cpp \
    qffitfunctionsspimfccsfw2csepadiff2coloracfg.cpp \
    qffitfunctionsspimfccsfw2csepadiff2coloracfr.cpp \
    qffitfunctionsspimfccsfw2csepadiff2colorccf.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_spimfcs.ts


INCLUDEPATH += ../../lib/


include(../../quickfit3_configqt.pri)

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE

