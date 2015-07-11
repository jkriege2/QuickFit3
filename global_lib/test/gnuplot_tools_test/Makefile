all: Release
debug: Debug
release: Release

	

LIB=../../../LIB/trunk
CC=g++


CFLAGS =  -Wall -fexceptions
#--enable-auto-import
LDFLAGS = 

Release: CFLAGS += -O2 -mtune=native -march=native -ffast-math -msse -msse2 -mfpmath=both -malign-double -mmmx -m3dnow  -ftree-vectorize -ftree-vectorizer-verbose=1

Debug: CC += -DDEBUG -g

EXECUTABLE=gnuplot_tools_test
SRC_FILE= main.cpp \
          ../../gnuplot_tools.cpp


SRC_FILE_O = $(subst .cpp,.o,$(SRC_FILE))


ifeq ($(findstring Msys,$(OS)),Msys)
PREFIX=/mingw
EXE_SUFFIX=.exe
SO_SUFFIX=.dll
SO_PATH=$(PREFIX)/bin
else
ifeq ($(findstring Windows,$(OS)),Windows)
PREFIX=/mingw
EXE_SUFFIX=.exe
SO_SUFFIX=.dll
SO_PATH=$(PREFIX)/bin
else
PREFIX=/usr/local
EXE_SUFFIX=
SO_SUFFIX=.so
SO_PATH=$(PREFIX)/lib
endif
endif

test: 
	make clean
	make Debug
	./${EXECUTABLE}$(EXE_SUFFIX)

test_debug: 
	make clean
	make Debug
	gdb ./${EXECUTABLE}$(EXE_SUFFIX)

Debug:  ${EXECUTABLE}$(EXE_SUFFIX)

Release:  ${EXECUTABLE}$(EXE_SUFFIX)

${EXECUTABLE}$(EXE_SUFFIX): ${SRC_FILE_O}
	$(CC) $(CFLAGS) -o $(EXECUTABLE)$(EXE_SUFFIX) ${SRC_FILE_O} $(LDFLAGS)

$(SRC_FILE_O): %.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	echo $(OS)
	rm -f *.exe
	rm -f ${EXECUTABLE}$(EXE_SUFFIX)
	rm -f ${SRC_FILE_O}
	rm -f *.o


