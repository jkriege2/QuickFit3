#
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#
#	last modification: $LastChangedDate: 2014-06-20 17:06:59 +0200 (Fr, 20 Jun 2014) $  (revision $Rev: 3266 $)
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


# only write new version files in release mode
# in debug mode: only make sure that the files exist, but do not update them


message(AUTOVERSIONING IN $$PWD)
#try to read compiler name and version
COMPILERVERSION=$$system($$QMAKE_CXX --version)
COMPILERVERSION_MACHINE=$$system($$QMAKE_CXX -dumpmachine)
message($$QMAKE_CXX  .$${DIR_SEPARATOR}tools$${DIR_SEPARATOR}qf3infotool$${DIR_SEPARATOR}main.cpp -o .$${DIR_SEPARATOR}output$${DIR_SEPARATOR}qf3infotool$${EXE_SUFFIX})
system($$QMAKE_CXX  .$${DIR_SEPARATOR}tools$${DIR_SEPARATOR}qf3infotool$${DIR_SEPARATOR}main.cpp -o .$${DIR_SEPARATOR}output$${DIR_SEPARATOR}qf3infotool$${EXE_SUFFIX})

# try to read the SVN version
SVNVERSION = $$system(svnversion)
contains(SVNVERSION,exported):SVNVERSION=
contains(SVNVERSION,exported):SVNVERSION=
isEmpty(SVNVERSION){
    # if SVN does not work, try git svn
    unix {
        SVNVERSION = $$system(git svn info | grep -i 'revision' | cut -d: -f2)
    }
}
isEmpty(SVNVERSION) {
    SVNVERSION = ---
}

DATESTR = $$system(.$${DIR_SEPARATOR}output$${DIR_SEPARATOR}qf3infotool$${EXE_SUFFIX} --date)
DATESTRYEAR = $$system(.$${DIR_SEPARATOR}output$${DIR_SEPARATOR}qf3infotool$${EXE_SUFFIX} --year)

message("COMPILE MODE: SVNVERSION is: $$SVNVERSION")
message("COMPILE MODE: build date is: $$DATESTR")
message("COMPILE MODE: compiler: $$COMPILERVERSION")
message("COMPILE MODE: compiler.machine: $$COMPILERVERSION_MACHINE")

system($$PWD$${DIR_SEPARATOR}output$${DIR_SEPARATOR}qf3infotool$${EXE_SUFFIX} --writedate $$PWD$${DIR_SEPARATOR}compiledate.h)

win32 {
    system(echo $$LITERAL_HASH define SVNVERSION \"$$SVNVERSION\"  > svnversion.h )
    system(echo $$LITERAL_HASH define COMPILER \"$$COMPILERVERSION (MACHINE: $$COMPILERVERSION_MACHINE)\"  > compiler.h )
} else {
    system(echo \'$$LITERAL_HASH define SVNVERSION \"$$SVNVERSION\"\'  > svnversion.h )
    system(echo \'$$LITERAL_HASH define COMPILER \"$$COMPILERVERSION (MACHINE: $$COMPILERVERSION_MACHINE)\"\'  > compiler.h )
}
#    system(echo $$LITERAL_HASH define COMPILEDATE \"$$DATESTR\"  > compiledate1.h )
#    system(echo \'$$LITERAL_HASH define COMPILEDATE \"$$DATESTR\"\'  > compiledate1.h )


message(AUTOVERSIONING DONE IN $$PWD)
