TEMPLATE = lib
CONFIG += plugin


TARGET = cam_testcamera

DEFINES += TARGETNAME=$$TARGET

include(../plugins.pri)
include(../../extlibs/tiff.pri)

DESTDIR = $$QFOUTPUT/plugins

DEPENDPATH += ./

LIBS += -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += cam_testcamera.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += cam_testcamera.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += cam_testcamera.qrc

TRANSLATIONS= ./translations/de.cam_testcamera.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



