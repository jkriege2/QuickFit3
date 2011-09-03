#ifndef _QFCIMG
#define _QFCIMG

#define cimg_display 0

#ifdef QF_HAS_PNG
#define cimg_use_png
#endif

#ifdef QF_HAS_LIBTIFF
#define cimg_use_tiff
#endif

#ifdef QF_HAS_LIBJPEG
#define cimg_use_jpeg
#endif

#ifdef QF_HAS_LIBZ
#define cimg_use_zlib
#endif

#ifdef QF_HAS_LAPACK
#define cimg_use_lapack
#endif

#include "cimg/CImg.h"

#endif //_QFCIMG
