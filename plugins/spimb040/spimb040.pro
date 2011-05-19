TEMPLATE = lib
CONFIG += plugin


TARGET = spimb040
DEPENDPATH += ./
DESTDIR = ../../output/plugins/
#DLLDESTDIR = $$DESTDIR
# ./
LIBS += -ltiff -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += spimb040.h \
           qfespimb040mainwindow.h \
           qfespimb040cameraview.h \
           qfespimb040cameraconfig.h \
           qfespimb040samplestageconfig.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/jkimage.h  \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += spimb040.cpp \
           qfespimb040mainwindow.cpp \
           qfespimb040cameraview.cpp \
           qfespimb040cameraconfig.cpp \
           qfespimb040samplestageconfig.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += spimb040.qrc

TRANSLATIONS= ../../output/assets/translations/de.spimb040.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg opengl
CONFIG += exceptions rtti stl

# DEFINITION FOR CImg Library
include( ../../extlibs/cimg.pri )
PRECOMPILED_HEADER += ../../extlibs/cimg.h



MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

