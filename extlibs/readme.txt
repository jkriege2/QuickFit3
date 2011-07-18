This directory contains external dependencies that may not be available on your system.
If so, you may use these, according to the information given below:



Andor driver:
~~~~~~~~~~~~~
  some tool scripts and headers for the Andor Camera SDK (commercial software !!!)



CImg:
~~~~~
  This image processing library is lightweight, i.e. it consists of a single header only. All needed files
  are stored in ./cimg/. you may include the file cimg.h into your sourcecode and cimg.pri into you project
  file, which will include everything and set all pathes as needed!

    in your project file, use this:  
        # DEFINITION FOR CImg Library
        include( ../extlibs/cimg.pri )
        PRECOMPILED_HEADER += ../extlibs/cimg.h

  As set in these files, cimg links against libpng and libz which should be available on your system! If not,
  you may change this accordingly.

    * http://cimg.sourceforge.net/



levmar:
~~~~~~~
  A library for Levenberg-Marquardt fits, used by a plugin. If this is not installed globally on your system,
  unpack it here in ./levmar/ and build it there. The files levmar.h as well as the link library liblevmar.a
  should reside in ./levmar/ afterwards, as this directory may be supplied as additional search directory for
  plugins that need this library and levmar.h is included as #include <levmar/levmar.h>. Add this to you
  project file (modified as appropriate!):

       INCLUDEPATH += ../../extlibs/
       LIBS += -L../../extlibs/ -llevmar

  Now compiling should either use your system-wide version, or the one in extlibs, if a system-wide is not found
  (system-precedence given by #include <...>  instead of #include "..." ).

    * http://www.ics.forth.gr/~lourakis/levmar/



libtiff:
~~~~~~~~
  A library for TIFF file access. If this is not installed globally on your system,
  unpack it here in ./libtiff/ and build it there. The files tiff*.h as well as the link library libtiff.a
  should reside in ./libtiff/ afterwards, as this directory may be supplied as additional search directory for
  plugins that need this library and tiff*.h is included as #include <tiff*.h>.
  Add this to you project file (modified as appropriate!):

       INCLUDEPATH += ../../extlibs/libtiff/
       LIBS += -L../../extlibs/libtiff/ -ltiff

  Now compiling should either use your system-wide version, or the one in extlibs, if a system-wide is not found
  (system-precedence given by #include <...>  instead of #include "..." ).
  
  A windows-binary for MinGW >=4.4 is also provided in a separate ZIP-file which you may unpack into your MinGW
  directory. Then you have a global version for windows.

    * http://www.libtiff.org/



MersenneTwister.h:
~~~~~~~~~~~~~~~~~~
  An implementation of the mersenne twister random number generator, lightweight in a single header file

    * http://www-personal.umich.edu/~wagnerr/MersenneTwister.h
