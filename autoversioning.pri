#
#    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center
#
#	last modification: $LastChangedDate: 2014-06-20 17:06:59 +0200 (Fr, 20 Jun 2014) $  (revision $Rev: 3266 $)
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


# only write new version files in release mode
# in debug mode: only make sure that the files exist, but do not update them


#try to read compiler name and version
COMPILERVERSION=$$system($$QMAKE_CXX --version)


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


win32 {
    # first we have to check whether a MS Windows style date command is called. If you call this without the /T option, it will
    # expect a new date on input, whereas the unix date command with the /T option will return an error message containing the
    # word "invalid". So we call date /T and check the output for "invalid". if it is present, we have a Unix-style date command,
    # otherwise the MS Windows date. Finally we can set DATESTR using the right command.
    HAS_WINDOWS_DATE=TRUE
    DATE_OUTPUT=$$system(date /T)
    contains(DATE_OUTPUT, "invalid"):HAS_WINDOWS_DATE=FALSE
    message($$DATE_OUTPUT)
    contains(HAS_WINDOWS_DATE, FALSE) {
        !isEmpty(QMAKE_SH) {
            DATESTR = $$system(date +%Y/%m/%d)
            DATESTRYEAR = $$system(date +%Y)
        }
    } else {
        DATESTR = $$system(date /T)
        DATESTRYEAR = $$system(date /T)
    }
} else {
    # on UNIX we just call the date command!
    DATESTR = $$system(date +%Y/%m/%d)
    DATESTRYEAR = $$system(date +%Y)
}
isEmpty(DATESTR) {
    DATESTR = _DATE_
    DATESTRYEAR = 2014
}
message("COMPILE MODE: SVNVERSION is: $$SVNVERSION")
message("COMPILE MODE: build date is: $$DATESTR")
message("COMPILE MODE: compiler: $$COMPILERVERSION")






win32 {
    system(echo $$LITERAL_HASH define SVNVERSION \"$$SVNVERSION\"  > svnversion.h )
    system(echo $$LITERAL_HASH define COMPILEDATE \"$$DATESTR\"  > compiledate.h )
    system(echo $$LITERAL_HASH define COMPILER \"$$COMPILERVERSION\"  > compiler.h )
} else {
    system(echo \'$$LITERAL_HASH define SVNVERSION \"$$SVNVERSION\"\'  > svnversion.h )
    system(echo \'$$LITERAL_HASH define COMPILEDATE \"$$DATESTR\"  > compiledate.h )
    system(echo \'$$LITERAL_HASH define COMPILER \"$$COMPILERVERSION\"\'  > compiler.h )
}


