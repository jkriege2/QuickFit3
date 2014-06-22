include(zlib.pri)
include(png.pri)
INCLUDEPATH += ../../extlibs/pixman/include/
LIBS += -L../../extlibs/pixman/lib/ -lpixman
DEFINES += QF_HAS_LIBPIXMAN
