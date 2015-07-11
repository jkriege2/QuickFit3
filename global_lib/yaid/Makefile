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
LIB_NAME = yaid
SO_NAME = lib$(LIB_NAME)$(SO_SUFFIX)

ifeq ($(findstring Msys,$(OS)),Msys)
SHARED_OPTS=-Wl,--out-implib,$(SO_NAME).a,-soname,$(SO_NAME)
DYNAMIC_LIB = $(LIB_NAME)$(SO_SUFFIX)
else
SHARED_OPTS=
DYNAMIC_LIB = $(LIB_NAME)
endif

SRC_LIB = yaid.cpp serialization.cpp
#HPP_LIB = yaid.hpp serialization.hpp exception.hpp types.hpp endianness.hpp
SRC_TEST = yaid_test.cpp
#HPP_TEST = yaid_test.hpp


TAR_SHARED_LIB = $(subst .cpp,.shared.o,$(SRC_LIB))
TAR_STATIC_LIB = $(subst .cpp,.static.o,$(SRC_LIB))
TAR_TEST = $(subst .cpp,.o,$(SRC_TEST))
DVI_TARGETS = $(subst .tex,.dvi,$(wildcard *.tex))
PS_TARGETS = $(subst .dvi,.ps,$(DVI_TARGETS))
PDF_TARGETS = $(subst .ps,.pdf,$(PS_TARGETS))

CC = g++


OPTIMIZATION_OPTS = -O2
CC_OPTS = -Wall $(OPTIMIZATION_OPTS)

CC_DYNAMIC_OPTS = $(CC_OPTS) -fPIC
LD_DYNAMIC_OPTS = $(CC_OPTS) -shared -fPIC -Wl,-rpath $(LIB_PATH) 
CC_STATIC_OPTS = $(CC_OPTS) $(OPTIMIZATION_OPTS)
LD_STATIC_OPTS = $(CC_STATIC_OPTS) -Wl,-rpath $(LIB_PATH) 


all: endianness.hpp lib$(LIB_NAME).a $(SO_NAME) test_static$(EXE_SUFFIX) test_shared$(EXE_SUFFIX) $(PS_TARGETS)

-include $(TAR_SHARED_LIB:.o=.d) $(TAR_STATIC_LIB:.o=.d) $(TAR_TEST:.o=.d)

$(TAR_SHARED_LIB): %.shared.o: %.cpp
	$(CC) $(CC_DYNAMIC_OPTS) -c -o $@ $<
	$(CC) -MM $(CC_OPTS) $*.cpp > $*.shared.d
	@mv -f $*.shared.d $*.d.tmp
	@sed -e 's|.*:|$*.shared.o:|' < $*.d.tmp > $*.shared.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.shared.d
	@rm -f $*.d.tmp

$(TAR_STATIC_LIB): %.static.o: %.cpp
	$(CC) $(CC_STATIC_OPTS) -c -o $@ $<
	$(CC) -MM $(CC_OPTS) $*.cpp > $*.static.d
	@mv -f $*.static.d $*.d.tmp
	@sed -e 's|.*:|$*.static.o:|' < $*.d.tmp > $*.static.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.static.d
	@rm -f $*.d.tmp

$(TAR_TEST): %.o: %.cpp
	$(CC) $(CC_OPTS) -c -o $@ $<
	$(CC) -MM $(CC_OPTS) $*.cpp > $*.d
	@mv -f $*.d $*.d.tmp
	@sed -e 's|.*:|$*.o:|' < $*.d.tmp > $*.d
	@sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@rm -f $*.d.tmp

lib$(LIB_NAME).a: $(TAR_STATIC_LIB) $(HPP_LIB)
	ar -cvur lib$(LIB_NAME).a $(TAR_STATIC_LIB)

$(SO_NAME): $(TAR_SHARED_LIB) $(HPP_LIB)
	$(CC) $(LD_DYNAMIC_OPTS) -o $(SO_NAME) $(TAR_SHARED_LIB) $(SHARED_OPTS)

install: lib$(LIB_NAME).a $(SO_NAME) $(HPP_LIB)
	mkdir -p $(INCLUDE_PATH)/yaid
	cp $(HPP_LIB) $(INCLUDE_PATH)/yaid/
	cp $(SO_NAME) $(SO_PATH)
	cp lib$(LIB_NAME).a $(LIB_PATH)

endianness.hpp: endianness_test$(EXE_SUFFIX)
	./$^ > endianness.hpp

endianness_test$(EXE_SUFFIX): endianness_test.cpp
	$(CC) $(CC_OPTS) -o $@ $^

test_static$(EXE_SUFFIX): $(TAR_TEST) lib$(LIB_NAME).a
	$(CC) $(CC_OPTS) $(TAR_TEST) -o $@ -static -Wl,--library-path,./ -L./ -l$(LIB_NAME)

test_shared$(EXE_SUFFIX): $(TAR_TEST) $(SO_NAME)
	$(CC) $(CC_OPTS) $(TAR_TEST) -o $@ -dynamic -Wl,-rpath ./ -Wl,--library-path,./ -L ./ -l$(DYNAMIC_LIB)

$(DVI_TARGETS): %.dvi: %.tex
	latex $<
	-bibtex $(subst .tex,,$<)
	latex $<
	latex $<

$(PS_TARGETS): %.ps: %.dvi
	dvips -o $@ $(subst .ps,.dvi,$@)
	-rm  $(subst .ps,.aux,$@) $(subst .ps,.log,$@) $(subst .ps,.bbl,$@) $(subst .ps,.blg,$@)

$(PDF_TARGETS): %.pdf: %.ps
	ps2pdf14 -dPDFsettings=/prepress -dAutoFilterColorImages=false -dColorImageFilter=/FlateEncode $<

pdf: $(PDF_TARGETS)

clean: 
	-rm -f *~
	-rm -f *.o
	-rm -f *.d
	-rm -f lib$(LIB_NAME).a
	-rm -f $(SO_NAME) $(SO_NAME).a
	-rm -f test_static
	-rm -f test_shared
	-rm -f endianness_test
	-rm -f *.exe
	-rm -f endianness.hpp
	-rm *.ps *.dvi *.pdf
