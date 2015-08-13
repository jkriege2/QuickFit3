#
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#	
#	 
#
#    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).
#
#    This software is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License (LGPL) as published by
#    the Free Software Foundation, either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License (LGPL) for more details.
#
#    You should have received a copy of the GNU Lesser General Public License (LGPL)
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#


# This one handles dllimport/dllexport directives.
DEFINES += QUAZIP_BUILD QUAZIP_STATIC

win32 {
    # workaround for qdatetime.h macro bug
    #DEFINES += NOMINMAX
}

include(./quazip/quazip-0.7.1/quazip.pri)

INCLUDEPATH += $$PWD/quazip/quazip-0.7.1

DEFINES += QF_HAS_QUAZIP QF_HAS_MINIZIP
