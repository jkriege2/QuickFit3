include(zlib.pri)
include(png.pri)
include(pixman.pri)
INCLUDEPATH += ../../extlibs/cairo/include/
LIBS += -L../../extlibs/cairo/lib/ -lcairo
DEFINES += QF_HAS_LIBCAIRO
