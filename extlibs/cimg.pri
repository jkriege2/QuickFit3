INCLUDEPATH += ../../extlibs/cimg/
HEADERS += ../../extlibs/cimg/CImg.h
win32:LIBS+=-lgdi32
#PRECOMPILED_HEADER += cimg.h
include (zlib.pri)
include (png.pri)
