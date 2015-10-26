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

TARGET = fitfunctions_tirfcs
DEPENDPATH += ./

include(../../qf3plugins.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfpfitfunctionstirfcs.h \
    ../interfaces/qffitfunctionconfigforglobalfitinterface.h \
    ../base_classes/imfcstools.h \
    qffitfunctionstirfcsdiffe2.h \
    qffitfunctionstirfcsadiffe2.h \
    qffitfunctionstirfccsdiffflowe2.h \
    qffitfunctionstirffccsfw2csep2ddiffxy2colorccf.h \
    qffitfunctionstirffccsfw2csep2ddiffxy2coloracfr.h \
    qffitfunctionstirffccsfw2csep2ddiffxy2coloracfg.h \
    qffitfunctionstirffccsfw2ddiffxy2colorccf.h \
    qffitfunctionstirffccsfw2ddiffxy2coloracfr.h \
    qffitfunctionstirffccsfw2ddiffxy2coloracfg.h \
    qffitfunctionstirffccsfw2ddiffxy2colorccf.h \
qffitfunctionstirfcsdiffe2_relbright.h \
    qffitfunctionstirffccsfw2dadiffxy2coloracfg.h \
    qffitfunctionstirffccsfw2dadiffxy2coloracfr.h \
    qffitfunctionstirffccsfw2dadiffxy2colorccf.h \
    qffitfunctionstirfcsadiff3de2.h \
    qffitfunctionstirfcsdiffflowe2.h \
qffitfunctionstirfcsdiff3de2.h \
qffitfunctionstirfcsdiff3de2_relbright.h \
    qffitfunctionstirfccsadiffe2.h \
    qffitfunctionstirfccsdiff.h
#\
#    qffitfunctionstirfcsdiff3de2.h \
#    qffitfunctionstirfcsdiff3de2_relbright.h \
#    qffitfunctionstirfcsadiff3dpixele2.h \
#    qffitfunctionstirfcsdiff3dpixele2.h \
#    qffitfunctionstirfcsdiff3dpixele2_relbright.h


SOURCES += qfpfitfunctionstirfcs.cpp\
    ../base_classes/imfcstools.cpp \
    qffitfunctionstirfcsdiffe2.cpp \
    qffitfunctionstirfcsadiffe2.cpp \
    qffitfunctionstirfccsdiffflowe2.cpp \
    qffitfunctionstirffccsfw2csep2ddiffxy2colorccf.cpp \
    qffitfunctionstirffccsfw2csep2ddiffxy2coloracfr.cpp \
    qffitfunctionstirffccsfw2csep2ddiffxy2coloracfg.cpp \
    qffitfunctionstirffccsfw2ddiffxy2colorccf.cpp \
    qffitfunctionstirffccsfw2ddiffxy2coloracfr.cpp \
    qffitfunctionstirffccsfw2ddiffxy2coloracfg.cpp \
qffitfunctionstirfcsdiffe2_relbright.cpp \
    qffitfunctionstirffccsfw2dadiffxy2coloracfg.cpp \
    qffitfunctionstirffccsfw2dadiffxy2coloracfr.cpp \
    qffitfunctionstirffccsfw2dadiffxy2colorccf.cpp \
    qffitfunctionstirfcsadiff3de2.cpp \
    qffitfunctionstirfcsdiffflowe2.cpp \
qffitfunctionstirfcsdiff3de2.cpp\
qffitfunctionstirfcsdiff3de2_relbright.cpp \
    qffitfunctionstirfccsadiffe2.cpp \
    qffitfunctionstirfccsdiff.cpp
# \
#    qffitfunctionstirfcsdiff3de2.cpp \
#    qffitfunctionstirfcsdiff3de2_relbright.cpp \
#    qffitfunctionstirfcsadiff3dpixele2.cpp \
#    qffitfunctionstirfcsdiff3dpixele2.cpp \
#    qffitfunctionstirfcsdiff3dpixele2_relbright.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ./translations/de.fitfunctions_tirfcs.ts


INCLUDEPATH += ../../lib/


include(../../quickfit3_configqt.pri)

#SRC_DISTRIBUTED.files = $$HEADERS \
#                        $$SOURCES \
#                        $$FORMS \
#                        $$RESOURCES \
#                        $$TRANSLATIONS \
#                        $$GPL3LICENSE

