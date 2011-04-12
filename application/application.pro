TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += . \
               ../lib/ \
               ../libqf3widgets/
INCLUDEPATH += . \
               ../lib/ \
               ../libqf3widgets/ \
               ../../../../LIB/trunk/ \
               ../../../../LIB/trunk/qt
DESTDIR = ../output
#QMAKE_LFLAGS_RPATH += .
QMAKE_RPATHDIR += ../output/

LIBS += -L../output/ -lquickfit3lib -lquickfit3widgets
# Input
HEADERS += mainwindow.h \
           version.h \
           optionsdialog.h

SOURCES += main.cpp \
           mainwindow.cpp \
           optionsdialog.cpp

FORMS =    optionsdialog.ui \
           about.ui \
           aboutplugins.ui

RESOURCES += quickfit3.qrc


TRANSLATIONS= ../output/translations/de.ts

INCLUDEPATH += ../../../../LIB/trunk/

RC_FILE = quickfit3.rc

QT += gui xml
CONFIG += console exceptions rtti stl

# DEFINITION FOR CImg Library
include( ../extlibs/cimg.pri )
PRECOMPILED_HEADER += ../extlibs/cimg.h



MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
