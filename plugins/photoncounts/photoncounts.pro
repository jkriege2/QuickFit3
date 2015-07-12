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


TARGET = photoncounts
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfrdrphotoncounts.h \
           qfrdrphotoncounts_data.h \
           qfrdrphotoncounts_dataeditor.h \
    ../interfaces/qfrdrphotoncountsinterface.h

SOURCES += qfrdrphotoncounts.cpp \
           qfrdrphotoncounts_data.cpp \
           qfrdrphotoncounts_dataeditor.cpp

FORMS = 

RESOURCES += qfrdrphotoncounts.qrc

TRANSLATIONS= ./translations/de.photoncounts.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/

include(../../quickfit3_configqtwidgets.pri)



ASSETSTESTDATA_FILES.files = ./examples/testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/testdata/

INSTALLS += ASSETSTESTDATA_FILES

