#-------------------------------------------------
#
# Project created by QtCreator 2012-02-16T22:30:13
#
#-------------------------------------------------

QT       += core network
QT += gui

DEFINES += QFLIB_LIBRARY
CONFIG   += console
CONFIG   -= app_bundle

TARGET = tools_test

TEMPLATE = app


SOURCES += main.cpp \
    ../../qftools.cpp \
    ../../programoptions.cpp \
    ../../matlabtools.cpp \
    ../../datatools.cpp \
    ../../csvtools.cpp \
    ../../binarydatatools.cpp \
    ../../qfdlgcsvparameters.cpp

HEADERS += \
    ../../qftools.h \
    ../../qfpreprocessormagic_private.h \
    ../../qfpreprocessormagic.h \
    ../../programoptions.h \
    ../../matlabtools.h \
    ../../datatools.h \
    ../../csvtools.h \
    ../../binarydatatools.h \
    ../../qfdlgcsvparameters.h

FORMS += \
    ../../qfdlg_csvparameters.ui
