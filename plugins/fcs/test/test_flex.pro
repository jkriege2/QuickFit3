#-------------------------------------------------
#
# Project created by QtCreator 2014-06-17T14:45:37
#
#-------------------------------------------------

QT       += core gui network


TARGET = test_flex
CONFIG   += console

TEMPLATE = app

DEFINES +=  QFLIB_LIBRARY

INCLUDEPATH += ../../../lib/ ../../../../../../LIB/trunk/


SOURCES += test_flex.cpp \
    ../../../lib/qfmathtools.cpp \
    ../../../lib/qftools.cpp \
    ../flex_sin_tools.cpp \
    ../../../../../../LIB/trunk/tools.cpp \
    ../../../lib/datatools.cpp \
    ../../../lib/csvtools.cpp \
    ../../../lib/matlabtools.cpp \
    ../../../lib/programoptions.cpp

HEADERS += \
    ../../../lib/qfmathtools.h \
    ../../../lib/qftools.h \
    ../flex_sin_tools.h \
    ../../../../../../LIB/trunk/tools.h \
    ../../../lib/datatools.h \
    ../../../lib/csvtools.h \
    ../../../lib/matlabtools.h \
    ../../../lib/programoptions.h
