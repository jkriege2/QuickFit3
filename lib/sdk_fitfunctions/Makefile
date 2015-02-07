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

#######################################################################################
## This Makefile compiles any .cpp file in its directory into a dll/so. 
## It is intended to be used to create QFLibraryFitFunction plugins for QuickFit 3.0
## It does not link against ANY external library, except the C/C++ stdlibs. This
## linking is done statically on Windows, so no depences on external assets should 
## be created!
#######################################################################################


Release: all
Debug: all
.PHONY = clean all

OS := $(shell uname -o)

ifeq ($(findstring Msys,$(OS)),Msys)
PREFIX=/mingw
SO_SUFFIX=.dll
else
PREFIX=/usr/local
SO_SUFFIX=.so
endif


ifeq ($(findstring Msys,$(OS)),Msys)
SHARED_OPTS=-static -lstdc++ -lgcc -shared  
else
SHARED_OPTS=-shared  -fPIC
endif

SOURCES=$(wildcard *.cpp)

TARGET = $(subst .cpp,$(SO_SUFFIX),$(SOURCES))

CC = g++

CC_DEFINES = -DQF3SFF_BUILD_DLL 

OPTIMIZATION_OPTS = -O3 -s
CC_OPTS = -Wall $(CC_DEFINES) $(OPTIMIZATION_OPTS)
Debug:CC_OPTS = -g -Wall $(CC_DEFINES)

all: $(TARGET)


%$(SO_SUFFIX): %.cpp quickfit-model-header.h quickfit-model-tools.h
	$(CC) $(CC_OPTS) $(SHARED_OPTS) -o $@ $<

clean:
	-rm -f *$(SO_SUFFIX)
	-rm -f *~
	-rm -f *.o
	-rm -f *.d
	-rm -f *.exe
