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

DEFINES += QF_HAS_SDFFLIB

HEADERS += \
    $$PWD/sdfflib/sequencer.h \
    $$PWD/sdfflib/sequencegenerator.h \
    $$PWD/sdfflib/sdfftools.h \
    $$PWD/sdfflib/sdffsequenceelements.h \
    $$PWD/sdfflib/sdffparser.h \
    $$PWD/sdfflib/sdffnodes.h \
    $$PWD/sdfflib/sdffhwcommands.h \
    $$PWD/sdfflib/sdfffunctiontools.h \
    $$PWD/sdfflib/sdfffunctions.h \
    $$PWD/sdfflib/nisequencegenerator.h \
    $$PWD/sdfflib/nidriver.h \
    $$PWD/sdfflib/driver.h \
    $$PWD/sdfflib/csvsequenceparser.h \
    $$PWD/sdfflib/channelmanager.h \
    $$PWD/sdfflib/basesequenceplotter.h \
    $$PWD/sdfflib/baseimporter.h \
    $$PWD/sdfflib/sequencererror.h \
    $$PWD/sdfflib/vartimingdriver.h\
    $$PWD/sdfflib/logfile.h\
    $$PWD/sdfflib/tools.h\
    $$PWD/sdfflib/datatable.h\
    $$PWD/sdfflib/jkiniparser.h \
    $$PWD/sdfflib/highrestimer.h

SOURCES += \
    $$PWD/sdfflib/sequencegenerator.cpp \
    $$PWD/sdfflib/sdfftools.cpp \
    $$PWD/sdfflib/sdffsequenceelements.cpp \
    $$PWD/sdfflib/sdffparser.cpp \
    $$PWD/sdfflib/sdffnodes.cpp \
    $$PWD/sdfflib/sdffhwcommands.cpp \
    $$PWD/sdfflib/sdfffunctions.cpp \
    $$PWD/sdfflib/nisequencegenerator.cpp \
    $$PWD/sdfflib/nidriver.cpp \
    $$PWD/sdfflib/driver.cpp \
    $$PWD/sdfflib/csvsequenceparser.cpp \
    $$PWD/sdfflib/channelmanager.cpp \
    $$PWD/sdfflib/basesequenceplotter.cpp \
    $$PWD/sdfflib/baseimporter.cpp \
    $$PWD/sdfflib/sequencererror.cpp \
    $$PWD/sdfflib/vartimingdriver.cpp\
    $$PWD/sdfflib/logfile.cpp\
    $$PWD/sdfflib/tools.cpp\
    $$PWD/sdfflib/datatable.cpp\
    $$PWD/sdfflib/jkiniparser.cpp\
    $$PWD/sdfflib/highrestimer.cpp


INCLUDEPATH += $$PWD/sdfflib/ 
