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
MORECFLAGS=" -mtune=generic -msse -msse2 -mmmx -m3dnow -mfpmath=sse "
if [ $MAKE_COMPILEFORLOCAL == "y" ] ; then
	if [ $MAKE_AGRESSIVEOPTIMIZATIONS == "y" ] ; then
		MORECFLAGS=" -mtune=native -msse -msse2 -mmmx -m3dnow -mfpmath=sse -ftree-vectorize -ftree-vectorizer-verbose=1"
	else
		MORECFLAGS=" -mtune=native -msse -msse2 -mmmx -m3dnow -mfpmath=sse "
	fi
else
	if [ $MAKE_AGRESSIVEOPTIMIZATIONS == "y" ] ; then
		MORECFLAGS=" -mtune=generic -msse -msse2 -mmmx -m3dnow -mfpmath=sse -ftree-vectorize -ftree-vectorizer-verbose=1"
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
		cp /mingw/bin/libstdc++*.dll ../output/
		cp /mingw/bin/libgcc*.dll ../output/
		cp /mingw/bin/pthread*.dll ../output/
		cp /mingw/bin/libpthread*.dll ../output/
		cp /mingw/bin/libwinpthread*.dll ../output/
		
		USEDQTMODULES="QtCore4 QtGui4 QtOpenGL4 QtScript4 QtScriptTools4 QtSvg4 QtXml4 QtNetwork4"
		USEDQTPLUGINS= "imageformats/*.dll"
		mkdir ../output/qtplugins
		for f in $USEDQTMODULES
		do
			cp "${QT_INFO_BIN}/${f}d.dll"  "../output/"
			cp "${QT_INFO_BIN}/${f}.dll"  "../output/"
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
	tar xvf zlib-1.2.8.tar.gz -C ./build/
	cd build/zlib-1.2.8
	ISMSYS=`uname -o`
	echo $ISMSYS
	if [ "$ISMSYS" != "${string/Msys/}" ] ; then
		BINARY_PATH='../../bin'
		INCLUDE_PATH='../../include'
		LIBRARY_PATH='../../lib'
		
		echo -e 'BINARY_PATH='$BINARY_PATH'\nINCLUDE_PATH='$INCLUDE_PATH'\nLIBRARY_PATH='$LIBRARY_PATH'\nLOC='-fPIC $MORECFLAGS|cat - ./win32/Makefile.gcc > ./Makefile.gcc
		
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


zlib_CFLAGS=
zlib_LDFLAGS="-lz"
if [ -e ${CURRENTDIR}/zlib/lib/libz.a ] ; then
	zlib_CFLAGS="-I${CURRENTDIR}/zlib/include"
	zlib_LDFLAGS="-L${CURRENTDIR}/zlib/lib -lz"
fi
echo -e "\n\n\n USING THESE zlib compiler commands:\nCFLAGS="
echo $zlib_CFLAGS
echo -e "\nLDFLAGS="
echo $zlib_LDFLAGS
echo -e "\n\n\n"


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
	./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/lmfit  CFLAGS="-fPIC ${MORECFLAGS}" CPPFLAGS="-fPIC ${MORECFLAGS}"	LDFLAGS="-fPIC"
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


lmfit5OK=-1
read -p "Do you want to build 'lmfit v5' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: lmfit v5                                                 --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd lmfit5
	mkdir build
	tar xvf lmfit-5.1.tar -C ./build/
	cd build/lmfit-5.1
        export LDFLAGS="${LDFLAGS} -fPIC "
        export CFLAGS="${CFLAGS} -fPIC "
        export CPPFLAGS="${CPPFLAGS} -fPIC"
	./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/lmfit5  CFLAGS=" ${MORECFLAGS}" CPPFLAGS=" ${MORECFLAGS}"	
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make -j${MAKE_PARALLEL_BUILDS}
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
		    mkdir ${CURRENTDIR}/lmfit5/include
		    cp ./lib/*.h ${CURRENTDIR}/lmfit5/include
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
	
	lmfit5OK=$libOK

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
		./configure --enable-static --disable-shared --prefix=${CURRENTDIR}/libpng LDFLAGS=-L${CURRENTDIR}/zlib/lib CFLAGS=-I${CURRENTDIR}/zlib/include CXXFLAGS=-I${CURRENTDIR}/zlib/include/
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
	tar xvf tiff-4.0.3.tar.gz -C ./build/
	cd build/tiff-4.0.3
	if [ -e ../../../zlib/lib/libz.a ] ; then
		./configure --enable-static --disable-shared --disable-jpeg --disable-jpeg --disable-old-jpeg --disable-jbig --with-zlib-include-dir=${CURRENTDIR}/zlib/include --with-zlib-lib-dir=${CURRENTDIR}/zlib/lib --prefix=${CURRENTDIR}/libtiff
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
	tar xvf gsl-1.16.tar.gz -C ./build/
	cd build/gsl-1.16
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
	read -p "Do you want to build 'libusb' (y/n)? " -n 1 INSTALL_ANSWER
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
			"-- BUILDING: libusb                                                   --\n"\
			"------------------------------------------------------------------------\n\n"\

			cd libusb
			mkdir build
			mkdir lib
			mkdir bin
			mkdir include
			tar xvf ./libusbx-1.0.16.tar.gz -C ./build/
			cd build/libusbx-1.0.16/
			./configure --prefix=${CURRENTDIR}/libusb  CFLAGS="${MORECFLAGS}" CPPFLAGS="${MORECFLAGS}"
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
			tar xvf ./libusb-compat-0.1.5.tar.gz -C ./build/
			cp fix/libusb-compat-0.1.5/* build/libusb-compat-0.1.5/libusb/
			cd build/libusb-compat-0.1.5/
			./configure --prefix=${CURRENTDIR}/libusb  CFLAGS=" ${MORECFLAGS}" CPPFLAGS="${MORECFLAGS}"  LIBUSB_1_0_CFLAGS="-I${CURRENTDIR}/libusb/include/libusb-1.0" LIBUSB_1_0_LIBS="-L${CURRENTDIR}/libusb/lib -lusb-1.0"
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
			cp libusb/bin/* ../output
			
			libusbOK=$libOK
		fi
	fi
fi







libnidaqmxOK=-5
ISMSYS=`uname -o`
echo $ISMSYS
if [ "$ISMSYS" != "${string/Msys/}" ] ; then
	libnidaqmxOK=-1
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






libandorOK=-5
ISMSYS=`uname -o`
echo $ISMSYS
if [ "$ISMSYS" != "${string/Msys/}" ] ; then
	libandorOK=-1
	read -p "Do you want to build 'libAndor' (windows only!!!) (y/n)? " -n 1 INSTALL_ANSWER
	echo -e  "\n"
	if [ $INSTALL_ANSWER == "y" ] ; then

		echo -e  "------------------------------------------------------------------------\n"\
		"-- BUILDING: libAndor                                                 --\n"\
		"------------------------------------------------------------------------\n\n"\
		echo -e "first we have to copy the file ATMCD32D.H from the Andor SDK to \n"
		echo -e "andor_win32 and andor_win64. Then ATMCD32D.DLL has to be copied \n"
		echo -e "to andor_win32 and atmcd64d.dll to andor_win64. Finally a script\n"
		echo -e "in each of these diretories will create the 32bit/64bit link libs\n"
		echo -e "which are needed to build the andor camera driver with mingw.\n\n"
	    read -p "please enter the directory of the Andor SDK "  ANDORSDK
	    echo -e  "\n"
	    read -p "Do you want to build 32bit or 64bit (3/6)? " -n 1 SELECTBITS
	    echo -e  "\n"
	    if [ $SELECTBITS == "6" ] ; then
            cd andor_win64
			cp $ANDORSDK/atmcd64d.dll .
			cp $ANDORSDK/ATMCD32D.H .
		else
            cd andor_win32
			cp $ANDORSDK/ATMCD32D.DLL .
			cp $ANDORSDK/ATMCD32D.H .
		fi
		
		./create_testcpp.sh
		libOK=$?
		cd ${CURRENTDIR}
		
		if [ $libOK -eq 0 ] ; then		
			libandorOK=0
		else
			libandorOK=-3
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
	tar xvf ./eigen-3.2.0.tar.gz -C ./build/
	cd build/eigen-eigen-ffa86ffb5570/
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



cimgOK=-1
read -p "Do you want to build 'cimg' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "------------------------------------------------------------------------\n"\
	"-- BUILDING: cimg                                                     --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd cimg
	mkdir build
	tar xvf ./CImg-1.5.6.tar.bz2 -C ./build/
	cd build/
	cp -r * ../
	libOK=$?
	if [ $libOK -ne 0 ] ; then		
		libOK=-4
	else
		libOK=0
	fi
	cd ../
	if [ $KEEP_BUILD_DIR == "n" ] ; then
		rm -rf build
	fi
	cd ${CURRENTDIR}
	
	cimgOK=$libOK

fi







libpixmanOK=-1
read -p "Do you want to build 'pixman' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: pixman                                                   --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd pixman
	mkdir build
	tar xvf pixman-0.32.4.tar.gz -C ./build/
	cd build/pixman-0.32.4
	./configure --enable-static --disable-shared --enable-libpng --disable-gtk --prefix=${CURRENTDIR}/pixman   CFLAGS="-fPIC ${MORECFLAGS} ${zlib_CFLAGS}" CPPFLAGS="-fPIC ${MORECFLAGS} ${zlib_CFLAGS}" LDFLAGS=" ${zlib_LDFLAGS}" PNG_LIBS="-lpng -L${CURRENTDIR}/libpng -L${CURRENTDIR}/libpng/lib" PNG_CFLAGS=" -I${CURRENTDIR}/libpng -I${CURRENTDIR}/libpng/include"
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
	
	libpixmanOK=$libOK

fi

libcairoOK=-1
read -p "Do you want to build 'cairo' (y/n)? " -n 1 INSTALL_ANSWER
echo -e  "\n"
if [ $INSTALL_ANSWER == "y" ] ; then
	echo -e  "\n------------------------------------------------------------------------\n"\
	"-- BUILDING: cairo                                                   --\n"\
	"------------------------------------------------------------------------\n\n"\

	cd cairo
	mkdir build
	
    # faked pkg-config ... hack by http://www.kyngchaos.com/macosx/build/cairo
	echo "if [ \"\$1\" = \"--atleast-pkgconfig-version\" ] ; then
	echo 1
	fi
	if [ \"\$1\" = \"--exists\" ] ; then
	echo 0
	return 1
	fi" > ./build/pkgconfigfake
	chmod +x ./build/pkgconfigfake
	
	tar xvf cairo-1.12.16.tar.gz -C ./build/
	cd build/cairo-1.12.16
	if [ "$ISMSYS" != "${string/Msys/}" ] ; then
	    cp ../../Makefile.in_hack ./Makefile.in
		./configure --verbose --enable-static --disable-shared --disable-dependency-tracking --enable-gobject=no --enable-trace=no --enable-xcb-shm=no --enable-xlib=no --enable-xlib-xrender=no --enable-xcb=no --enable-egl=no --enable-glx=no --enable-wgl=no --enable-ft=no  --enable-fc=no --disable-xlib --without-x --disable-quartz-font --disable-quartz --disable-valgrind --prefix=${CURRENTDIR}/cairo   CFLAGS="-fPIC ${MORECFLAGS} ${zlib_CFLAGS} -I${CURRENTDIR}/pixman/include/ -I${CURRENTDIR}/pixman/include/pixman -I${CURRENTDIR}/pixman/include/pixman-1 " CPPFLAGS="-fPIC ${MORECFLAGS} ${zlib_CFLAGS} -I${CURRENTDIR}/pixman/include/ -I${CURRENTDIR}/pixman/include/pixman -I${CURRENTDIR}pixman/include/pixman-1 " LDFLAGS=" ${zlib_LDFLAGS} -L${CURRENTDIR}/pixman/lib -lpixman-1 " png_REQUIRES=libpng png_LIBS="-lpng -L${CURRENTDIR}/libpng -L${CURRENTDIR}/libpng/lib " png_CFLAGS=" -I${CURRENTDIR}/libpng/include " pixman_CFLAGS=" -I${CURRENTDIR}pixman/include/pixman-1" pixman_LIBS=" -L${CURRENTDIR}/pixman/lib -lpixman-1" PKG_CONFIG="${CURRENTDIR}/cairo/build/pkgconfigfake"
	else 
        ./configure --verbose --enable-static --disable-shared --disable-dependency-tracking  --prefix=${CURRENTDIR}/cairo   CFLAGS="-fPIC ${MORECFLAGS} ${zlib_CFLAGS} -I${CURRENTDIR}/pixman/include/ -I${CURRENTDIR}/pixman/include/pixman -I${CURRENTDIR}/pixman/include/pixman-1 " CPPFLAGS="-fPIC ${MORECFLAGS} ${zlib_CFLAGS} -I${CURRENTDIR}/pixman/include/ -I${CURRENTDIR}/pixman/include/pixman -I${CURRENTDIR}pixman/include/pixman-1 " LDFLAGS=" ${zlib_LDFLAGS} -L${CURRENTDIR}/pixman/lib -lpixman-1 " png_REQUIRES=libpng png_LIBS="-lpng -L${CURRENTDIR}/libpng -L${CURRENTDIR}/libpng/lib " png_CFLAGS=" -I${CURRENTDIR}/libpng/include " pixman_CFLAGS=" -I${CURRENTDIR}pixman/include/pixman-1" pixman_LIBS=" -L${CURRENTDIR}/pixman/lib -lpixman-1"	
	fi
	libOK=$?
	if [ $libOK -eq 0 ] ; then
		make 
		
		libOK=$?
		if [ $libOK -eq 0 ] ; then		
			make install
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
	
	libcairoOK=$libOK

fi

echo -e  "\n------------------------------------------------------------------------\n"\
"-- BUILD RESULTS                                                       --\n"\
"------------------------------------------------------------------------\n\n"\

print_result "Qt DLLs copy" $qtOK
print_result "zlib" $zlibOK
print_result "lmfit" $lmfitOK
print_result "lmfit v5" $lmfit5OK
print_result "levmar" $levmarOK
print_result "libpng" $libpngOK
print_result "libtiff" $libtiffOK
print_result "gsl" $libgslOK
print_result "libusb" $libusbOK
print_result "libNIDAQmx" $libnidaqmxOK
print_result "libNIDAQmx" $libandorOK
print_result "eigen" $eigenOK
print_result "cimg" $cimgOK
print_result "pixman" $libpixmanOK
print_result "cairo" $libcairoOK


