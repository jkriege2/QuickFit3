QuickFit 3.0
~~~~~~~~~~~~

0. Building dependencies
   Some plugins need additional libraries. All of these should be available in the ./extlibs/ 
   directory. The ./extlibs/redme.txt file gives further hints on how to build them. For
   win32 systems, binaries are supplied in the 'win32_binaries' subdirectory of the respective
   library directory. The library directory contains a zip'ed source package (in a tested version)
   for each dependency! The supplied build script uses this source package to build the lib.

1. Building Debug:
       qmake "CONFIG-=release" "CONFIG+=debug" quickfit3.pro
       make
   After execution of these instructions all files an embedded plugins will be compiled.
   The executable and libraries are created in ./output/. The plugin DLLs are copied to
   ./output/plugins/. An additional
       make install
   will copy all assets and documentations to the ./output/ directory



2. Building Release:
       qmake "CONFIG+=release" "CONFIG-=debug" quickfit3.pro
       make
   After execution of these instructions all files an embedded plugins will be compiled.
   The executable and libraries are created in ./output_release/. The plugin DLLs are 
   copied to ./output/plugins/. An additional
       make install
   will copy all assets and documentations to the ./output/ directory


3. Deploying QuickFit
   In the output directory a deploy_debug.sh/deploy_release.sh script exists, which
   packs all necessary files into a .zip package. If you want to deploy Release and have 
   built Debug before it, make sure to call 
       make distclean 
   in the QuickFit 3 base directory!
