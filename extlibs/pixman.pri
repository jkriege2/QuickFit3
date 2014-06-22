include(zlib.pri)
include(png.pri)
INCLUDEPATH += ../../extlibs/pixman/include/pixman-1/ ../../extlibs/pixman/include/
LIBS += -L../../extlibs/pixman/lib/ -lpixman-1
DEFINES += QF_HAS_LIBPIXMAN
