all: Release

LIB=../../../LIB/trunk
CC=g++

CFLAGS =  -Wall -DHAVE_INLINE
LDFLAGS = 

Release: CFLAGS += -O2 

Debug: CC += -DDEBUG -g

EXECUTABLE=example_image_server
SRC_FILE= tcpipserver.cpp \
          example_image_server.cpp \
		  tinytiffwriter.cpp


SRC_FILE_O = $(subst .cpp,.o,$(SRC_FILE))

# -L/usr/X11R6/lib -lm -lpthread -lX11

ifeq ($(findstring Msys,$(OS)),Msys)
	PREFIX=/mingw
	EXE_SUFFIX=.exe
	SO_SUFFIX=.dll
	SO_PATH=$(PREFIX)/bin
	LDFLAGS += -lwsock32
else
	ifeq ($(findstring Windows,$(OS)),Windows)
		PREFIX=/mingw
		EXE_SUFFIX=.exe
		SO_SUFFIX=.dll
		SO_PATH=$(PREFIX)/bin
		LDFLAGS += -lwsock32
	else
		PREFIX=/usr/local
		EXE_SUFFIX=
		SO_SUFFIX=.so
		SO_PATH=$(PREFIX)/lib
		LDFLAGS += -lm
	endif
endif

CPPFLAGS = $(CFLAGS)


Debug:  ${EXECUTABLE}$(EXE_SUFFIX)

Release:  ${EXECUTABLE}$(EXE_SUFFIX)

${EXECUTABLE}$(EXE_SUFFIX): ${SRC_FILE_O}
	$(CC) $(CFLAGS) -o $(EXECUTABLE)$(EXE_SUFFIX) ${SRC_FILE_O} $(LDFLAGS)

$(SRC_FILE_O): %.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<
	


cleanRelease: clean
cleanDebug: clean

clean:
	echo $(OS)
	rm -f ${EXECUTABLE}$(EXE_SUFFIX)
	rm -f ${SRC_FILE_O}


