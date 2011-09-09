# only write new version files in release mode
# in debug mode: only make sure that the files exist, but do not update them



# try to read the SVN version
SVNVERSION = $$system(svnversion)
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
        }
    } else {
        DATESTR = $$system(date /T)
    }
} else {
    # on UNIX we just call the date command!
    DATESTR = $$system(date +%Y/%m/%d)
}
isEmpty(DATESTR) {
    DATESTR = ---
}
message("RELEASE MODE: svnversion is: $$SVNVERSION")
message("RELEASE MODE: build date is: $$DATESTR")






win32 {
    system(echo $$LITERAL_HASH define SVNVERSION \"$$SVNVERSION\"  > svnversion.h )
    system(echo $$LITERAL_HASH define COMPILEDATE \"$$DATESTR\"  > compiledate.h )
} else {
    system(echo \'$$LITERAL_HASH define SVNVERSION \"$$SVNVERSION\"\'  > svnversion.h )
    system(echo \'$$LITERAL_HASH define COMPILEDATE \"$$DATESTR\"\'  > compiledate.h )
}


