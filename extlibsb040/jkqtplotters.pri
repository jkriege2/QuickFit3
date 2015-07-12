#
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
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


HEADERS +=  $$PWD/JKQtPlotter/jkqtpbaseplotter.h \
           $$PWD/JKQtPlotter/jkqtpdatastorage.h \
           $$PWD/JKQtPlotter/jkqtpelements.h \
           $$PWD/JKQtPlotter/jkqtpgeoelements.h \
           $$PWD/JKQtPlotter/jkqtmathtext.h \
           $$PWD/JKQtPlotter/jkqtpbaseelements.h \
           $$PWD/JKQtPlotter/jkqtplotter.h \
           $$PWD/JKQtPlotter/jkqtptools.h \
           $$PWD/JKQtPlotter/jkqtpmathparser.h \
           $$PWD/JKQtPlotter/jkqtfastplotter.h \
            $$PWD/JKQtPlotter/jkqtpimagetools.h \
            $$PWD/JKQtPlotter/jkqtpimageelements.h \
            $$PWD/JKQtPlotter/jkqtpoverlayelements.h \
            $$PWD/JKQtPlotter/jkqtpparsedfunctionelements.h 
           

SOURCES += $$PWD/JKQtPlotter/jkqtpbaseplotter.cpp \
           $$PWD/JKQtPlotter/jkqtpdatastorage.cpp \
           $$PWD/JKQtPlotter/jkqtpelements.cpp \
           $$PWD/JKQtPlotter/jkqtpgeoelements.cpp \
           $$PWD/JKQtPlotter/jkqtmathtext.cpp \
           $$PWD/JKQtPlotter/jkqtpbaseelements.cpp \
           $$PWD/JKQtPlotter/jkqtplotter.cpp \
           $$PWD/JKQtPlotter/jkqtptools.cpp \
           $$PWD/JKQtPlotter/jkqtpmathparser.cpp \
           $$PWD/JKQtPlotter/jkqtfastplotter.cpp \
           $$PWD/JKQtPlotter/jkqtpimagetools.cpp \
           $$PWD/JKQtPlotter/jkqtpimageelements.cpp \
           $$PWD/JKQtPlotter/jkqtpoverlayelements.cpp \
           $$PWD/JKQtPlotter/jkqtpparsedfunctionelements.cpp

RESOURCES += $$PWD/JKQtPlotter/jkqtpbaseplotter.qrc 
INCLUDEPATH += $$PWD/JKQTPlotter/ \
               $$PWD/libtiff_tools/
