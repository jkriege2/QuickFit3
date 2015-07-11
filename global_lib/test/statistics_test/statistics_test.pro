TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -Werror=return-type -msse -msse2 -mmmx -m3dnow -mfpmath=sse -ftree-vectorize -funroll-loops
!macx:QMAKE_CXXFLAGS += -malign-double

include(../../../../FCSTOOLS/trunk/QuickFit3/extlibs/eigen.pri)

DEFINES+= STATISTICS_TOOLS_MAY_USE_EIGEN3

TARGET = statistics_tools_test

SOURCES += \
    test.cpp \
    ../../statistics_tools.cpp

OTHER_FILES +=

HEADERS += \
    ../../statistics_tools.h

