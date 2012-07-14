#!/bin/sh
echo -e "This script will create a MinGW .a link library for the NI DAQmx driver.\n"\
        "It automatizes the steps described in http://www.mingw.org/wiki/MSVC_and_MinGW_DLLs\n"\
		"using the tool pexports from http://sourceforge.net/projects/mingw/files/MinGW/Extension/pexports/pexports-0.44-1/\n\n"\
		"see also: http://forums.ni.com/t5/Multifunction-DAQ/How-Can-I-Use-The-NI-DAQmx-ANSI-C-Function-Library-With-GCC-on/td-p/195781/page/2\n\n"\
		"\033[0;33mIf this script does not succeed it is possible that it did not find the files it needs\n"\
		"at the expected positions in the system. these files are needed:\n"\
		"  - nicaiu.dll (usually in /c/windows/system32/) \n"\
		"  - NIDAQmx.h (usually in /c/Program Files (x86)/National Instruments/NI-DAQ/DAQmx ANSI C Dev/\n"\
		"               or /c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/\n"\
		"               or /c/Programme/National Instruments/NI-DAQ/DAQmx ANSI C Dev/\n"\
		"               or /c/Programme (x86)/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include)\n"\
		"If any of these files is not found and this script fails, please copy them into this directory \n"\
		"and rerun the script.\n"\
		"\033[0m \n\n"\

echo -e "1. trying to copy NIDAQmx.h ...\n"
cp -f NIDAQmx.h .
OK=$?
if [ $OK -ne 0 ]; then
	cp -f "/c/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include/NIDAQmx.h" .
	OK=$?
fi
if [ $OK -ne 0 ]; then
	cp -f "/c/Programme/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include/NIDAQmx.h" .
	OK=$?
fi
if [ $OK -ne 0 ]; then
	cp -f "/c/Program Files (x86)/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include/NIDAQmx.h" .
	OK=$?
fi
if [ $OK -ne 0 ]; then
	cp -f "/c/Programme (x86)/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include/NIDAQmx.h" .
fi

if [[ ! -f NIDAQmx.h ]]; then
	echo -e "\033[0;31m ERROR: did not find file NIDAQmx.h \033[0m \n" 
fi
		
		
echo -e "2. try to find (and copy) nicaiu.dll here ..."
cp -f nicaiu.dll .
OK=$?
if [ $OK -ne 0 ]; then
	cp -f /c/windows/system32/nicaiu.dll .
fi



if [ -f NIDAQmx.h ]; then
	if [ -f nicaiu.dll ]; then
		echo -e "\033[1;32m OK \033[0m \n" 
		echo -e "3. running pexports ...\n"
		./tools_win32/pexports.exe -h NIDAQmx.h  nicaiu.dll | sed "s/^_//" > nicaiu.def
		OK=$?
		if [ $OK -eq 0 ]; then
			echo -e "4. running dlltool ...\n"
			dlltool -d nicaiu.def -llibNIDAQmx.a -k

			OK=$?
			if [ $OK -eq 0 ]; then
				echo -e "\033[1;32m INSTALL of libNIDAQmx SUCCESSFULL! \033[0m \n" 
				exit 0
			fi
		fi
	fi
fi

echo -e "\033[0;31m ERROR: the file NIDAQmx.h or nicaiu.dll (or both) are not present \033[0m \n" 
exit 1
