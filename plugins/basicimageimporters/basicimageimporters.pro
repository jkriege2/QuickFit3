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

TARGET = importers_basicimages
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfbasicimageimporters.h \
           qfimagereaderlibtiff.h \
    ../../../../../LIB/trunk/libtiff_tools.h \
    qfimagereaderradhard.h \
    ../../../../../LIB/trunk/crc_ccitt.h \
    ../../../../../LIB/trunk/crc.h \
    ../../../../../LIB/trunk/frame.h \
    qfbasicimageimportersoptionswidget.h \
    qfimagereadertinytiff.h \
    ../../../../../LIB/trunk/tinytiffwriter.h \
    ../../../../../LIB/trunk/tinytiffreader.h \
    qfimagereaderdiffusion4binary.h \
    ../../../../../LIB/trunk/helper.h

SOURCES += qfbasicimageimporters.cpp\
           qfimagereaderlibtiff.cpp \
    ../../../../../LIB/trunk/libtiff_tools.cpp \
    qfimagereaderradhard.cpp \
    qfbasicimageimportersoptionswidget.cpp \
    qfimagereadertinytiff.cpp \
    ../../../../../LIB/trunk/tinytiffwriter.cpp \
    ../../../../../LIB/trunk/tinytiffreader.cpp \
    qfimagereaderdiffusion4binary.cpp

FORMS = \
    qfbasicimageimportersoptionswidget.ui

RESOURCES += \
    basicimageimporters.qrc

TRANSLATIONS= ./translations/de.importers_basicimages.ts


INCLUDEPATH += ../../lib/


TINYTIFF_FILES.files = ../../../../../LIB/trunk/tinytiffwriter.cpp \
                       ../../../../../LIB/trunk/tinytiffreader.cpp  \
                       ../../../../../LIB/trunk/tinytiffwriter.h \
                       ../../../../../LIB/trunk/tinytiffreader.h \
                       $$GPL3LICENSE
TINYTIFF_FILES.path = $${QFOUTPUT}/source/tinytiff/
tinytiff_reader_test_FILES.files = ../../../../../LIB/trunk/test/tinytiff_reader_test/*.*
tinytiff_reader_test_FILES.path = $${QFOUTPUT}/source/tinytiff/test/tinytiff_reader_test
tinytiffwriter_test_FILES.files = ../../../../../LIB/trunk/test/tinytiffwriter_test/*.*
tinytiffwriter_test_FILES.path = $${QFOUTPUT}/source/tinytiff/test/tinytiffwriter_test

INSTALLS += TINYTIFF_FILES \
            tinytiff_reader_test_FILES \
            tinytiffwriter_test_FILES



QT += core
CONFIG += exceptions rtti stl
