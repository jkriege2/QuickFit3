include(zlib.pri)
INCLUDEPATH += ../../extlibs/libtiff/include/
LIBS += -L../../extlibs/libtiff/lib/ -ltiff
DEFINES += QF_HAS_LIBTIFF cimg_use_tiff
