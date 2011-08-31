# This file presets some basic configuration options. If you want to change one of them,
# create a file quickfit.inc in the base directory and change the options there!
#

# location of the global configuration directory on unix systems (QuickFit will add 'quickfit3/' to this string)
GLOBALCONFIGDIR = /usr/share/

# indicate whether the library lapack is present, if so, you can define the link options in LAPACK_LINK (e.g. "-lblas -llapack")
# and the include directories in LAPACK_INCLUDE
HAS_LAPACK = false
LAPACK_LINK =
LAPACK_INCLUDE =

# indicate whether the library blas is present, if so, you can define the link options in BLAS_LINK (e.g. "-lblas -lf2c -lm")
# and the include directories in BLAS_INCLUDE
HAS_BLAS = false
BLAS_LIB =
BLAS_INCLUDE =


exists(quickfit.inc):include(quickfit.inc)

DEFINES += "GLOBALCONFIGDIR=\"$$GLOBALCONFIGDIR\""
