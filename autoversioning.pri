# try to read the SVN version
# in the output, search for the line 'Revision: XXX' where XXX is the current Head revision
SVNVERSION = $$system(svnversion)
isEmpty(SVNVERSION){
    # if SVN does not work, try git svn
    unix {
        SVNVERSION = $$system(git svn info -r HEAD ../ | grep 'evision' | cut -d: -f2)
    }
}
!isEmpty(SVNVERSION) {
    system(echo ' $$LITERAL_HASH define SVNVERSION \" $$SVNVERSION \" ' > svnversion.h )
}
isEmpty(SVNVERSION) {
    system(echo ' $$LITERAL_HASH define SVNVERSION \"---\" ' > svnversion.h )
}
