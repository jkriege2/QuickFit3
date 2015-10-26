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

include("../bitdepth.pri")

exists(andor.inc):include(andor.inc)

win32 {
    message("Andor-driver for Windows, $${BIT_DEPTH}-bits")
    contains( ANDOR_WIN64, true )|contains(BIT_DEPTH, 64) {
      LIBS+= -L$$PWD/andor_win64
      INCLUDEPATH += $$PWD/andor_win64
      message("  --> $${PWD}/andor_win64")
    } else {
      LIBS+= -L$$PWD/andor_win32
      INCLUDEPATH += $$PWD/andor_win32
      message("  --> $${PWD}/andor_win32")
    }
}

LIBS += -landor

HEADERS +=

SOURCES +=



