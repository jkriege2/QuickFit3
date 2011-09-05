This directory contains external dependencies that may not be available on your system.

If there is a .pri file in this directory for your library that one should solve all your problems.
So simply include it into your project and you should not need to add other options to your .pro file!

There is a script build_dependencies.sh that tries to build all necessary libraries that are NEEDED to
compile QuickFit and B040's plugins. The libraries will be built (as far as possible) without any
external dependencies and in static versions, so compiling QuickFit with these libs should leed to an
application that is fully statically linked.

A note on LAPACK: Some of the external libs may use lapack/blas. Currently no dependency and no plugins
absolutely NEED any of the two, so they are not part of the dependencies here. If you have them on your 
system, you may use them though by telling the build_dependencies.sh (it asks for them) and setting
the quickfit.inc file in the main directory.

At the end of the file you'll find some additional information on some of the libraries.



Andor driver:
~~~~~~~~~~~~~
  some tool scripts and headers for the Andor Camera SDK (commercial software !!!)
  If you need to build the andor plugin on win32/MinGW you can use the link library in
  the directory andor_win32 and also the header file ATMCD32D.H there. If you want to 
  build the andor plugin on Linux you need to install the Andor-SDK before, so the libraries 
  are available system-wide


libusb:
~~~~~~~
  The build script currently does NOT build libusb. But you will need it only for some of the
  device drivers, not for the core fitting components. But on win32 the build script can uncompress
  and locally install the supplied binary, taken from:
    * http://sourceforge.net/apps/trac/libusb-win32/wiki


CImg:
~~~~~
  This image processing library is lightweight, i.e. it consists of a single header only. All needed files
  are stored in ./cimg/. you may include the file cimg.h into your sourcecode and cimg.pri into you project
  file, which will include everything and set all pathes as needed!
    * http://cimg.sourceforge.net/

  in your project file, use this:  
	# DEFINITION FOR CImg Library
	include( ../extlibs/cimg.pri )
	
  in your C++-files include the file cimg.h from the ./extlibs/ library directly, as this also cares for 
  eventually available additional libraries (libtiff, lapack, libpng, ...) that can be used by CImg!
	

	
levmar:
~~~~~~~
  A library for Levenberg-Marquardt fits, used by a plugin. 
    * http://www.ics.forth.gr/~lourakis/levmar/


lmfit:
~~~~~~
  A small Levenberg-Marquardt library, which is very fast, but does not support boundary constraints.
    * http://joachimwuttke.de/lmfit/


libtiff:
~~~~~~~~
  A library for TIFF file access. 
    * http://www.libtiff.org/


libpng:
~~~~~~~
  A library for PNG file access. 
    * http://www.libpng.org/pub/png/libpng.html


GSL:
~~~~
  The GNU Scientific library, including many tools for math and computations, also a CBLAS implementation.
    * http://www.gnu.org/s/gsl/


MersenneTwister.h:
~~~~~~~~~~~~~~~~~~
  An implementation of the mersenne twister random number generator, lightweight in a single header file
    * http://www-personal.umich.edu/~wagnerr/MersenneTwister.h
