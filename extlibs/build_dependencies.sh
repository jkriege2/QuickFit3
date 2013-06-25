#! /bin/bash

function print_result {
	if [ $2 == -1 ]; then
		echo -e "building '$1': \033[0;33m NOT SELECTED \033[0m" 
	fi
	if [ $2 == -2 ]; then
		echo -e "building '$1': \033[0;31m ERROR IN CONFIGURE \033[0m" 
	fi
	if [ $2 == -3 ]; then
		echo -e "building '$1': \033[0;31m ERROR IN MAKE STEP \033[0m" 
	fi
	if [ $2 == -4 ]; then
		echo -e "building '$1': \033[0;31m ERROR IN INSTALL STEP \033[0m" 
	fi
	if [ $2 == -5 ]; then
		echo -e "building '$1': \033[0;33m INSTALL SCRIPT DOES NOT SUPPORT THIS ON YOUR PLATFORM \033[0m" 
	fi
	if [ $2 == 0 ]; then
		echo -e "building '$1': \033[1;32m OK \033[0m" 
	fi
}



echo -e "========================================================================\n"\
"= QuickFit 3 dependency build script                                   =\n"\
"========================================================================\n\n"\
"This script will try to build all the dependency libraries \n"\
"needed for QuickFit. All libraries will be built in static linking mode\n"\
"and stay inside their subdirectory of this extlibs directory, so they\n"\
"don't interfere with your system-libraries. The .pri-files in this di-\n"\
"rectory include the libraries in a way that preferences these libraries \n"\
"here!\n\nFirst we need to set some basics for the builds:"\

read -p "Do you want to build keep the build directories (y/n)? " -n 1 KEEP_BUILD_DIR
echo -e  "\n"
read -p "How many parallel builds do you want to use in make (1/2/3/...)? " -n 1  MAKE_PARALLEL_BUILDS
echo -e  "\n"
read -p "Do you want to use more agressive optimizations for the built libraries (y/n)? " -n 1  MAKE_AGRESSIVEOPTIMIZATIONS
echo -e  "\n"
read -p "Do you want to optimize libraries for your local machine? (y/n)? " -n 1  MAKE_COMPILEFORLOCAL
echo -e  "\n"

#sh ../output/get_bit_depth.sh
MORECFLAGS="-m128bit-long-double -mtune=generic -msse -msse2 -mmmx -m3dnow -mfpmath=sse "
if [ $MAKE_COMPILEFORLOCAL == "y" ] ; then
	if [ $MAKE_AGRESSIVEOPTIMIZATIONS == "y" ] ; then
		MORECFLAGS="-m128bit-long-double -mtune=native -msse -msse2 -mmmx -m3dnow -mfpmath=sse -ftree-vectorize -ftree-vectorizer-verbose=5"
	else
		MORECFLAGS="-m128bit-long-double -mtune=native -msse -msse2 -mmmx -m3dnow -mfpmath=sse "
	fi
else
	if [ $MAKE_AGRESSIVEOPTIMIZATIONS == "y" ] ; then
		MORECFLAGS="-m128bit-long-double -mtune=generic -msse -msse2 -mmmx -m3dnow -mfpmath=sse -ftree-vectorize -ftree-vectorizer-verbose=5"
	fi
fi

CURRENTDIR=${PWD}
QT_INFO_LIBS=`qmake -query QT_INSTALL_LIBS`
QT_INFO_BIN=`qmake -query QT_INSTALL_BINS`
QT_INFO_PLUGINS=`qmake -query QT_INSTALL_PLUGINS`
QT_INFO_INSTALLDIR=`qmake -query QT_INSTALL_PREFIX`
QT_INFO_VERSION=`qmake -query QT_VERSION`
echo -e "\n\nbuilding for\n    Qt version ${QT_INFO_VERSION}\n       in ${QT_INFO_INSTALLDIR}\n\n"

ISMSYS=`uname -o`
echo $ISMSYS
if [ "$ISMSYS" != "${string/Msys/}" ] ; then
	echo -e "building in MSys environment on Windows!\n\n"
fi

qtOK=-5
if [ "$ISMSYS" != "${string/Msys/}" ] ; then
	qtOK=-1
	read -p "Do you want to copy 'Qt DLLs' (y/n)? " -n 1 INSTALL_ANSWER
	echo -e  "\n"
	if [ $INSTALL_ANSWER == "y" ] ; then
		echo -e  "------------------------------------------------------------------------\n"\
		"-- COPYING: Qt DLLs                                                   --\n"\
		"------------------------------------------------------------------------\n\n"
		
		cp /mingw/bin/mingwm10.dll ../output/
		cp /mingw/bin/libstdc++-6.dll ../output/
		
		USEDQTMODULES="QtCore4 QtGui4 QtOpenGL4 QtScript4 QtScriptTools4 QtSvg4 QtXml4"
		USEDQTPLUGINS= "imageformats/*.dll"
		mkdir ../output/qtplugins
		for f in $USEDQTMODULES
		do
			cp "${QT_INFO_BIN}/${f}d.dll"  "../output/qtplugins"
			cp "${QT_INFO_BIN}/${f}.dll"  "../output/qtplugins"
		done
		for f in $USEDQTPLUGINS
		do
			cp "${QT_INFO_PLUGINS}/${f}"  "../output/qtplugins"
			cp "${QT_INFO_PLUGINS}/${f}"  "../output/qtplugins"
		done
		qtOK=0
	fi
fi



zlibOK=-1
read -p "Do you want to build 'zlib' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "------------------------------------------------------------------------\n"\
	"-- BUILDING: zlib                                                     --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd zlib
	mkdir build
	tar xvf zlib-1.2.5.tar.gz -C ./build/
	cd build/zlib-1.2.5
	ISMSYS=`uname -o`
	echo $ISMSYS
	if [ "$ISMSYS" != "${string/Msys/}" ] ; then
		BINARY_PATH='../../bin'
		INCLUDE_PATH='../../include'
		LIBRARY_PATH='../../lib'
		
		echo -e 'BINARY_PATH='$BINARY_PATH'\nINCLUDE_PATH='$INCLUDE_PATH'\nLIBRARY_PATH='$LIBRARY_PATH'\nLOC='$MORECFLAGS|cat - ./win32/Makefile.gcc > ./Makefile.gcc
		
		MAKEFILE="Makefile.gcc"
	else
		./configure --static --prefix=${CURRENTDIR}/zlib  CFLAGS="${MORECFLAGS}" CPPFLAGS="${MORECFLAGS}"
		MAKEFILE="Makefile"
	fi
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS} -f $MAKEFILE
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make -j${MAKE_PARALLEL_BUILDS} install -f $MAKEFILE
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	zlibOK=$libOK

fi


lmfitOK=-1
read -p "Do you want to build 'lmfit' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: lmfit                                                    --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd lmfit
	mkdir build
	tar xvf lmfit-3.2.tar -C ./build/
	cd build/lmfit-3.2
	export LDFLAGS="${LDFLAGS} -fPIC "
        export CFLAGS="${CFLAGS} -fPIC "
        export CPPFLAGS="${CPPFLAGS} -fPIC"
	./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/lmfit  CFLAGS="-fPIC ${MORECFLAGS}" CPPFLAGS="-fPIC ${MORECFLAGS}"
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
		    mkdir ${CURRENTDIR}/lmfit/include
		    cp ./lib/*.h ${CURRENTDIR}/lmfit/include
			make -j${MAKE_PARALLEL_BUILDS} install
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	lmfitOK=$libOK

fi



read -p "Do you want to build 'levmar' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
levmarOK=-1
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: levmar                                                    --\n"\
	"------------------------------------------------------------------------\n\n"\

	
	cd levmar
	mkdir build
	tar xvf levmar-2.6.tar.gz -C ./build/
	cd build/levmar-2.6

	
	
	SKIP="0"
	read -p "Do you have 'lapack' on your system (y/n)? " -n 1 HAVE_LAPACK
	echo -e  "\n"
	if [ $HAVE_LAPACK == "y" ] ; then
	
		echo -e  "Possibilities to link agains LAPACK/BLAS:\n"\
		"  1) simple LAPACK+BLAS (-llapack -lblas)\n"\
		"  2) simple LAPACK+CBLAS (-llapack -lcblas)\n"\
		"  3) F2C'd LAPACK (-llapack -lblas -lf2c)\n"\
		"  4) ATLAS LAPACK (-L/usr/local/atlas/lib -llapack -lcblas -lf77blas -latlas -lf2c)\n"\
		"\n  5) build without lapack\n"\
		"\n  6) skip levmar\n"

		read -p "Please enter your option: " -n 1 i

		#  #define HAVE_LAPACK is already set in the library header, so we don't need to change it there
		# we only need to alter the Makefile!
		#case $i in
		#1) sed 's/LAPACKLIBS=/LAPACKLIBS=-llapack -lblas#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile ;;
		#2) sed 's/LAPACKLIBS=/LAPACKLIBS=-llapack -lcblas#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile ;;
		#3) sed 's/LAPACKLIBS=/LAPACKLIBS=-llapack -lblas -lf2c#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile ;;
		#4) sed 's/LAPACKLIBS=/LAPACKLIBS=-L\/usr\/local\/atlas\/lib -llapack -lcblas -lf77blas -latlas -lf2c#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile ;;
		#5) HAVE_LAPACK="n" ;;
		#6) SKIP="1" ;;
		#esac
		case $i in
		1) echo "LAPACKLIBS=-llapack -lblas" > Makefile; cat ../../buildscript_tools/Makefile >> Makefile; ;;
		2) echo "LAPACKLIBS=-llapack -lcblas" > Makefile; cat ../../buildscript_tools/Makefile >> Makefile; ;;
		3) echo "LAPACKLIBS=-llapack -lblas -lf2c" > Makefile; cat ../../buildscript_tools/Makefile >> Makefile; ;;
		4) echo "LAPACKLIBS=-L\/usr\/local\/atlas\/lib -llapack -lcblas -lf77blas -latlas -lf2c" > Makefile; cat ../../buildscript_tools/Makefile >> Makefile; ;;
		5) HAVE_LAPACK="n" ;;
		6) SKIP="1" ;;
		esac
	fi	
	if [ $HAVE_LAPACK == "n" ] ; then
		# and alter the Makefile
		cp ../../buildscript_tools/levmar.h .
		echo "LAPACKLIBS=" > Makefile; cat ../../buildscript_tools/Makefile >> Makefile; 
	else
		cp ../../buildscript_tools/levmar.h ./levmar2.h
		sed 's/\/\/\#define HAVE\_LAPACK/\#define HAVE\_LAPACK/g' levmar2.h >> levmar.h
		rm ./levmar2.h
		
	fi 
	echo "OPTIMIZATIONFLAGS=${MORECFLAGS}"|cat - Makefile > Makefile.tmp && mv Makefile.tmp Makefile 

	if [ $SKIP == "0" ] ; then

		make -j${MAKE_PARALLEL_BUILDS} 
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		

			mkdir ../../lib
			mkdir ../../include
			mkdir ../../include/levmar
			cp *.a ../../lib/
			cp levmar.h ../../include/
			cp levmar.h ../../include/levmar/
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
		levmarOK=$libOK
	fi
	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}

fi


libpngOK=-1
read -p "Do you want to build 'libpng' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: libpng                                                    --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd libpng
	mkdir build
	tar xvf libpng-1.5.4.tar.gz -C ./build/
	cd build/libpng-1.5.4
	if [ -e ../../../zlib/lib/libz.a ] ; then
		./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/libpng LDFLAGS=-L${CURRENTDIR}/zlib/lib/ CFLAGS=-I${CURRENTDIR}/zlib/include/ CXXFLAGS=-I${CURRENTDIR}/zlib/include/
	else
		./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/libpng
	fi
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make -j${MAKE_PARALLEL_BUILDS} install
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	libpngOK=$libOK

fi



libtiffOK=-1
read -p "Do you want to build 'libtiff' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: libtiff                                                   --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd libtiff
	mkdir build
	tar xvf tiff-3.9.5.tar.gz -C ./build/
	cd build/tiff-3.9.5
	if [ -e ../../../zlib/lib/libz.a ] ; then
		./configure --enable-static --disable-shared --disable-jpeg --disable-jpeg --disable-old-jpeg --disable-jbig --with-zlib-include-dir=${CURRENTDIR}/zlib/include/ --with-zlib-lib-dir=${CURRENTDIR}/zlib/lib/ --prefix=${CURRENTDIR}/libtiff
	else
		./configure --enable-static --disable-shared --disable-jpeg --disable-jpeg --disable-old-jpeg --disable-jbig --prefix=${CURRENTDIR}/libtiff
	fi
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make -j${MAKE_PARALLEL_BUILDS} install
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	libtiffOK=$libOK

fi



libgslOK=-1
read -p "Do you want to build 'gsl' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: gsl                                                       --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd gsl
	mkdir build
	tar xvf gsl-1.15.tar.gz -C ./build/
	cd build/gsl-1.15
	./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/gsl   CFLAGS="-fPIC ${MORECFLAGS}" CPPFLAGS="-fPIC ${MORECFLAGS}"
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make -j${MAKE_PARALLEL_BUILDS} install
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			fi
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	libgslOK=$libOK

fi



libusbOK=-5
if [ "$ISMSYS" != "${string/Msys/}" ] ; then
	libusbOK=-1
	read -p "Do you want to build 'libusb-win32' (windows only!!!) (y/n)? " -n 1 INSTALL_ANSWER
	echo -e  "\n"
	if [ $INSTALL_ANSWER == "y" ] ; then
		read -p "Do you want to use prebuilt win32 'libusb' (y/n)? " -n 1 INSTALL_ANSWER
		echo -e  "\n"
		if [ $INSTALL_ANSWER == "y" ] ; then
			echo -e  "------------------------------------------------------------------------\n"\
			"-- INSTALLING: precompiled libusb (win32)                             --\n"\
			"------------------------------------------------------------------------\n\n"\

			cd libusb
			mkdir build
			mkdir lib
			mkdir bin
			mkdir include
			tar xvf ./win32_binary/libusb-win32-bin-1.2.5.0.tar.gz -C ./build/
			cd build/libusb-win32-bin-1.2.5.0/
			cp ./include/* ../../include
			cp ./include/lusb0_usb.h ../../include/usb.h
			libOK=$?
			if [ $libOK -ne 0 ] ; then		
				libOK=-4
			else 
				cp ./lib/gcc/* ../../lib
				libOK=$?
				if [ $libOK -ne 0 ] ; then		
					libOK=-4
				else
					cp -r ./bin/* ../../bin
					libOK=$?
					if [ $libOK -ne 0 ] ; then		
						libOK=-4
					fi
				fi
			fi

			cd ../../
			if [ $KEEP_BUILD_DIR == "n" ] ; then
				rm -rf build
			fi
			cd ${CURRENTDIR}
			
			libusbOK=$libOK
		else	
			echo -e  "------------------------------------------------------------------------\n"\
			"-- BUILDING: libusb (win32)                                           --\n"\
			"------------------------------------------------------------------------\n\n"\

			cd libusb
			mkdir build
			mkdir lib
			mkdir bin
			mkdir include
			tar xvf ./libusb-win32-src-1.2.6.0.tar.gz -C ./build/
			cd build/libusb-win32-src-1.2.6.0/
			make -j${MAKE_PARALLEL_BUILDS}
			
			libOK=$?
			if [ $libOK -eq 0 ] ; then		
				rm ../../include/usb.h
				echo -e "#include \"lusb0_usb.h\"\n\n" >> ../../include/usb.h
				cp ./src/lusb0_usb.h ../../include
				cp *.a ../../lib
				cp *.dll ../../bin				
				cp *.def ../../lib				
				cp *.def ../../bin				
				cp *.exe ../../bin				
			else
				libOK=-3
			fi
			cd ../../
			if [ $KEEP_BUILD_DIR == "n" ] ; then
				rm -rf build
			fi
			cd ${CURRENTDIR}
			
			libusbOK=$libOK
		fi
	fi
fi







libnidaqmxOK=-5
ISMSYS=`uname -o`
echo $ISMSYS
if [ "$ISMSYS" != "${string/Msys/}" ] ; then
	libusbOK=-1
	read -p "Do you want to build 'libNIDAQmx' (windows only!!!) (y/n)? " -n 1 INSTALL_ANSWER
	echo -e  "\n"
	if [ $INSTALL_ANSWER == "y" ] ; then

		echo -e  "------------------------------------------------------------------------\n"\
		"-- BUILDING: libnidaqmxOK                                             --\n"\
		"------------------------------------------------------------------------\n\n"\

		cd nidaqmx
		./makenidaqmx.sh
		libOK=$?
		cd ${CURRENTDIR}
		
		if [ $libOK -eq 0 ] ; then		
			libnidaqmxOK=0
		else
			libnidaqmxOK=-3
		fi
	fi
fi




eigenOK=-1
read -p "Do you want to build 'eigen' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "------------------------------------------------------------------------\n"\
	"-- BUILDING: eigen                                                    --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd eigen
	mkdir build
	mkdir doc
	mkdir include
	mkdir include/Eigen
	tar xvf ./eigen-3.0.4.tar.bz2 -C ./build/
	cd build/eigen-eigen-13a11181fc5a/
	cp -r ./Eigen/* ../../include/Eigen
	libOK=$?
	cp -r ./doc/* ../../doc
	if [ $libOK -ne 0 ] ; then		
		libOK=-4
	else
		libOK=0
	fi
	cd ../../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	eigenOK=$libOK

fi






libqt3dOK=-1
read -p "Do you want to build 'qt-3d (for Qt4)' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: qt-3d (for Qt4)                                          --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd qt3d_qt4
	mkdir build
	tar xvf qt-qt3d-qt4.tar.gz -C ./build/
	cd build/qt-qt3d
	qmake qt3d.pro "CONFIG+=release debug"
	libOK=$?
	if [ $libOK -eq 0 ] ; then
	    echo "patching the assip lib inside Qt3D!!!"
		ASSIMPFILES="./3rdparty/assimp/include/aiTypes.h"
		for i in $ASSIMPFILES; do
			sed 's/ ::strlen/std::strlen/g' $i > $i.temp | mv $i.temp $i 
			sed 's/ ::strcmp/std::strcmp/g' $i > $i.temp | mv $i.temp $i 
			sed 's/<math.h>/<cmath>/g' $i > $i.temp | mv $i.temp $i 
			sed 's/#define AI_TYPES_H_INC/#define AI_TYPES_H_INC \
			#include <cstring>/g' $i > $i.temp | mv $i.temp $i 
		done
	
		make 
		make release
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			cp "${QT_INFO_BIN}/Qt3Dd.dll"  "../../../../output/"
			cp "${QT_INFO_BIN}/Qt3D.dll"  "../../../../output/"
			ibOK=0
		else
			libOK=-3
		fi
	else
	    libOK=-2
	fi
	

	cd ../../
	#if [ $KEEP_BUILD_DIR == "n" ] ; then
	#	rm -rf build
	#fi
	cd ${CURRENTDIR}
	
	libqt3dOK=$libOK

fi





echo -e  "\n------------------------------------------------------------------------\n"\
"-- BUILD RESULTS                                                       --\n"\
"------------------------------------------------------------------------\n\n"\

print_result "Qt DLLs copy" $qtOK
print_result "zlib" $zlibOK
print_result "lmfit" $lmfitOK
print_result "levmar" $levmarOK
print_result "libpng" $libpngOK
print_result "libtiff" $libtiffOK
print_result "gsl" $libgslOK
print_result "libusb" $libusbOK
print_result "libNIDAQmx" $libnidaqmxOK
print_result "eigen" $eigenOK
print_result "qt-3d (for Qt4)" $libqt3dOK