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
read -p "how many parallel builds do you want to use in make (1/2/3/...)? " -n 1  MAKE_PARALLEL_BUILDS
echo -e  "\n"

CURRENTDIR=${PWD}

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
		#cp ./win32/Makefile.gcc .
		BINARY_PATH='../../bin'
		INCLUDE_PATH='../../include'
		LIBRARY_PATH='../../lib'
		
		echo -e 'BINARY_PATH='$BINARY_PATH'\nINCLUDE_PATH='$INCLUDE_PATH'\nLIBRARY_PATH='$LIBRARY_PATH|cat - ./win32/Makefile.gcc > ./Makefile.gcc
		
		MAKEFILE="Makefile.gcc"
	else
		./configure --static --prefix=${CURRENTDIR}/zlib
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
	
	./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/lmfit
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
	tar xvf levmar-2.5.tgz -C ./build/
	cd build/levmar-2.5

	
	
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
		case $i in
		1) sed 's/LAPACKLIBS=/LAPACKLIBS=-llapack -lblas#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile ;;
		2) sed 's/LAPACKLIBS=/LAPACKLIBS=-llapack -lcblas#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile ;;
		3) sed 's/LAPACKLIBS=/LAPACKLIBS=-llapack -lblas -lf2c#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile ;;
		4) sed 's/LAPACKLIBS=/LAPACKLIBS=-L\/usr\/local\/atlas\/lib -llapack -lcblas -lf77blas -latlas -lf2c#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile ;;
		5) HAVE_LAPACK="n" ;;
		6) SKIP="1" ;;
		esac
	fi	
	if [ $HAVE_LAPACK == "n" ] ; then
		# we have to undefine #define HAVE_LAPACK in the library header
		sed 's/#define HAVE_LAPACK/\/*#define HAVE_LAPACK*\//g' levmar.h > levmar.h.temp | mv levmar.h.temp levmar.h
		# and alter the Makefile
		sed 's/LAPACKLIBS=/LAPACKLIBS=#/g' Makefile > Makefile.temp | mv Makefile.temp Makefile
	fi 

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
	./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/gsl
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
ISMSYS=`uname -o`
echo $ISMSYS
if [ "$ISMSYS" != "${string/Msys/}" ] ; then
	libusbOK=-1
	read -p "Do you want to build 'libusb' (y/n)? " -n 1 INSTALL_ANSWER
	echo -e  "\n"
	if [ $INSTALL_ANSWER == "y" ] ; then
		echo -e  "------------------------------------------------------------------------\n"\
		"-- BUILDING: libusb (win32)                                           --\n"\
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

	fi
fi

echo -e  "\n------------------------------------------------------------------------\n"\
"-- BUILD RESULTS                                                       --\n"\
"------------------------------------------------------------------------\n\n"\

print_result "zlib" $zlibOK
print_result "lmfit" $lmfitOK
print_result "levmar" $levmarOK
print_result "libpng" $libpngOK
print_result "libtiff" $libtiffOK
print_result "gsl" $libgslOK
print_result "libusb" $libusbOK