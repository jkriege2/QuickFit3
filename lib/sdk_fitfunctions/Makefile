Release: all
Debug: all
.PHONY = clean all

OS := $(shell uname -o)

ifeq ($(findstring Msys,$(OS)),Msys)
PREFIX=/mingw
SO_SUFFIX=.dll
else
PREFIX=/usr/local
SO_SUFFIX=.so
endif


ifeq ($(findstring Msys,$(OS)),Msys)
SHARED_OPTS=-static -lstdc++ -lgcc -shared  -fPIC
else
SHARED_OPTS=-shared  -fPIC
endif

SOURCES=$(wildcard *.cpp)

TARGET = $(subst .cpp,$(SO_SUFFIX),$(SOURCES))

CC = g++

CC_DEFINES = -DBUILD_DLL 

OPTIMIZATION_OPTS = -O3 -s
CC_OPTS = -Wall $(CC_DEFINES) $(OPTIMIZATION_OPTS)
Debug:CC_OPTS = -g -Wall $(CC_DEFINES)

all: $(TARGET)


%$(SO_SUFFIX): %.cpp
	$(CC) $(CC_OPTS) $(SHARED_OPTS) -o $@ $<

clean:
	-rm -f *$(SO_SUFFIX)
	-rm -f *~
	-rm -f *.o
	-rm -f *.d
	-rm -f *.exe
