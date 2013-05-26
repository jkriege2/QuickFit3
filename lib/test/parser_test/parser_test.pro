#-------------------------------------------------
#
# Project created by QtCreator 2013-04-27T17:07:59
#
#-------------------------------------------------

QT       += core gui

DEFINES +=QFLIB_TEST QFMATHPARSER_DEBUGFUNCTIONNAMES #QFMATHPARSER_BYTECODESTACK_ERRORCHECKING

TARGET = parser_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    ../../qftools.cpp \
    ../../qfmathtools.cpp \
    ../../qfmathparser.cpp

HEADERS += \
    ../../qftools.h \
    ../../qfmathtools.h \
    ../../qfmathparser.h \
    ../../lib_imexport.h
