QuickFit 3.0
~~~~~~~~~~~~

1. Installing a build environment:

   1.1. Linux
   Make sure you have current Qt libraries (>=4.6) including the development packages on your system.
   You'll also need a C++-compiler (we compile using the GNU Compiler Collection). For the autoversioning
   you also need a subversion client (especially the command svnversion). Finally we advise you to use
   QtCreator as an IDE, as then you can simply load the .pro file and compile hapily away ;-)
   If you prefer a different IDE, you will have to call qmake to create the Makefiles and then make.

   1.2. Win32
   First install a current version of the QtSDK with at least the components:
     MinGW, Qt-libs (>=4.6), QtCreator (>2.2.0)
   Let's assume for now that you installe in C:\development\QtSDK	 
   For the autoversioning you'll need subversion which you can download as windows binary from:
      http://sourceforge.net/projects/win32svn/ 
	  http://sourceforge.net/projects/win32svn/files/1.6.17/svn-win32-1.6.17.zip
   simply unpack the contents of the binary zip-file into C:\development\QtSDK\mingw
   For building the dependency libs you'll also need MSys which provides a unix'oid shell and
   other Unix tools needed to execute configure scripts. You can get it here:
      http://sourceforge.net/projects/mingw/files/MSYS/BaseSystem/msys-core/msys-1.0.11/MSYS-1.0.11.exe
   Install it into C:\development\QtSDK\mingw\msys. In the configuration step give the path to MinGW as
   'c:/development/QtSDK/mingw'.
   Now you should be ready to build the dependencies and QuickFit 3
   
   1.3 MacOS
   Install the Qt SDK, compile the dependencies and then compile QuickFit.
   
   

2. Building dependencies
   Some plugins need additional libraries. All of these should be available in the ./extlibs/ 
   directory. The ./extlibs/redme.txt file gives further hints on how to build them. For
   win32 systems, binaries are supplied in the 'win32_binaries' subdirectory of the respective
   library directory. The library directory contains a zip'ed source package (in a tested version)
   for each dependency! The supplied build script uses this source package to build the lib.

   
   
3. Building Debug:
       qmake "CONFIG-=release" "CONFIG+=debug" quickfit3.pro
       make
   After execution of these instructions all files an embedded plugins will be compiled.
   The executable and libraries are created in ./output/. The plugin DLLs are copied to
   ./output/plugins/. An additional
       make install
   will copy all assets and documentations to the ./output/ directory



4. Building Release:
       qmake "CONFIG+=release" "CONFIG-=debug" quickfit3.pro
       make
   After execution of these instructions all files an embedded plugins will be compiled.
   The executable and libraries are created in ./output_release/. The plugin DLLs are 
   copied to ./output/plugins/. An additional
       make install
   will copy all assets and documentations to the ./output/ directory


   
5. Deploying QuickFit
   In the output directory a deploy_debug.sh/deploy_release.sh script exists, which
   packs all necessary files into a .zip package. If you want to deploy Release and have 
   built Debug before it, make sure to call 
       make distclean 
   in the QuickFit 3 base directory!
