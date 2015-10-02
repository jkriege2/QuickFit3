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

LIBS+= -L$$PWD/nidaqmx -lNIDAQmx
INCLUDEPATH += $$PWD/nidaqmx

DEFINES += QF_HAS_NIDAQMX

!exists($$PWD/nidaqmx/libNIDAQmx.a) {
    error("didn't find the link-library libNIDAQmx.a for the National Instruments DAQmx driver. If you don't have that library, add a line 'QF3CONFIG+=nonidaqmx' to your quickfit.inc file to prevent plugins that rely on NIDAQmx to be built!!!")
}
