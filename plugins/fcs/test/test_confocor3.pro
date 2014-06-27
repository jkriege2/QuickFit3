#-------------------------------------------------
#
# Project created by QtCreator 2014-06-17T14:45:37
#
#-------------------------------------------------

QT       += core gui network


TARGET = test_confocor3
CONFIG   += console

TEMPLATE = app

DEFINES +=  QFLIB_LIBRARY

INCLUDEPATH += ../../../lib/ ../../../../../../LIB/trunk/


SOURCES += test_confocor3.cpp \
    ../../../lib/qfmathtools.cpp \
    ../../../lib/qftools.cpp \
    ../confocor3tools.cpp \
    ../../../../../../LIB/trunk/tools.cpp \
    ../../../lib/datatools.cpp \
    ../../../lib/csvtools.cpp \
    ../../../lib/matlabtools.cpp \
    ../../../lib/programoptions.cpp \
    ../../../lib/qfdlgcsvparameters.cpp

HEADERS += \
    ../../../lib/qfmathtools.h \
    ../../../lib/qftools.h \
    ../confocor3tools.h \
    ../../../../../../LIB/trunk/tools.h \
    ../../../lib/datatools.h \
    ../../../lib/csvtools.h \
    ../../../lib/matlabtools.h \
    ../../../lib/programoptions.h \
    ../../../lib/qfdlgcsvparameters.h

FORMS += \
    ../../../lib/qfdlg_csvparameters.ui
