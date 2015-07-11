
.PHONY = clean all pdf

OS := $(shell uname -o)

ifeq ($(findstring Msys,$(OS)),Msys)
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


INCLUDE_PATH=$(PREFIX)/include
LIB_PATH=$(PREFIX)/lib
BIN_PATH=$(PREFIX)/bin

ifeq ($(findstring Msys,$(OS)),Msys)
SHARED_OPTS=-Wl,--out-implib,$(SO_NAME).a,-soname,$(SO_NAME)
else
SHARED_OPTS=
endif

SRC_TEST = ../../highrestimer.cpp ../../statistics_tools.cpp ../../tools.cpp main.cpp
#HPP_TEST = yaid_test.hpp


TAR_TEST = $(subst .cpp,.o,$(SRC_TEST))

CC = g++

OPTIMIZATION_OPTS = -O0
CC_OPTS = -Wall $(OPTIMIZATION_OPTS) -g

CC_STATIC_OPTS = $(CC_OPTS) $(OPTIMIZATION_OPTS)


all:  test$(EXE_SUFFIX)



$(TAR_TEST): %.o: %.cpp
	$(CC) $(CC_OPTS) -c -o $@ $<



test$(EXE_SUFFIX): $(TAR_TEST)
	$(CC) $(CC_OPTS) $(TAR_TEST) -o $@ $(LD_OPTS)



clean:
	-rm -f *~
	-rm -f *.o
	-rm -f *.d
	-rm -f test
	-rm -f *.exe
