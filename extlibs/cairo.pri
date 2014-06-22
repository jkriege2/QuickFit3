INCLUDEPATH += ../../extlibs/cairo/include/cairo/ ../../extlibs/cairo/include/ cairo
LIBS += -L../../extlibs/cairo/lib/ -lcairo
DEFINES += QF_HAS_LIBCAIRO
include(pixman.pri)
win32:LIBS += -lgdi32
