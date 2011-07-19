# only write new version files in release mode
# in debug mode: only make sure that the files exist, but do not update them

!debug {
    # try to read the SVN version
    SVNVERSION = $$system(svnversion)
    isEmpty(SVNVERSION){
        # if SVN does not work, try git svn
        unix {
            SVNVERSION = $$system(git svn info -r HEAD ../ | grep 'evision' | cut -d: -f2)
        }
    }
    !isEmpty(SVNVERSION) {
        system(echo ' $$LITERAL_HASH define SVNVERSION \" $$SVNVERSION \" ' > svnversion.h )
    } else {
        system(echo ' $$LITERAL_HASH define SVNVERSION \"---\" ' > svnversion.h )
    }

    win32 {
        !isEmpty(QMAKE_SH) {
            DATESTR = $$system(date)
        } else {
            DATESTR = $$system(date /T)
        }
    } else {
        DATESTR = $$system(date)
    }

    !isEmpty(DATESTR) {
        system(echo ' $$LITERAL_HASH define COMPILEDATE \" $$DATESTR \" ' > compiledate.h )
    } else {
        system(echo ' $$LITERAL_HASH define COMPILEDATE \"---\" ' > compiledate.h )
    }
} else {
    message("DEBUG MODE: here we do not read the new svn version, but only make sure that the autoversioning files do exist")
    message("DEBUG MODE: compile in RELEASE mode in order to update autoversion!!!")
    !exists(compiledate.h):system(echo ' $$LITERAL_HASH define COMPILEDATE \"---\" ' > compiledate.h )
    !exists(svnversion.h):system(echo ' $$LITERAL_HASH define SVNVERSION \"---\" ' > svnversion.h )
}
