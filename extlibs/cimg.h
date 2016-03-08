#ifndef _QFCIMG
#define _QFCIMG

#define cimg_display 0

#ifdef QF_HAS_PNG
#  ifndef cimg_use_png
#    define cimg_use_png
#  endif
#endif

#ifdef QF_HAS_LIBTIFF
#  ifndef cimg_use_tiff
#    define cimg_use_tiff
#  endif
#endif

#ifdef QF_HAS_LIBJPEG
#  ifndef cimg_use_jpeg
#    define cimg_use_jpeg
#  endif
#endif

#ifdef QF_HAS_LIBZ
#  ifndef cimg_use_zlib
#    define cimg_use_zlib
#  endif
#endif

#ifdef QF_HAS_LAPACK
#  ifndef cimg_use_lapack
#    define cimg_use_lapack
#  endif
#endif

#include "CImg.h"

#endif //_QFCIMG
