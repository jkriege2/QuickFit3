TEMPLATE = lib
CONFIG += plugin

TARGET = cam_systemcam
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/cimg.pri)

DESTDIR = $${QFOUTPUT}/plugins/


# Input
HEADERS += qfecamsystemcamera.h \
           ../interfaces/qfextensioncamera.h \
        ../../../../../LIB/trunk/videocapture.h \
    syscamconfigdialog.h

SOURCES += qfecamsystemcamera.cpp \
    syscamconfigdialog.cpp


unix {
  LIBS +=
  SOURCES += ../../../../../LIB/trunk/videocapture_v4l2.cpp
}

win32 {
  LIBS += -lgdi32 -luser32 -lkernel32 -lvfw32
  SOURCES += ../../../../../LIB/trunk/videocapture_vfw.cpp
}


FORMS = \
    syscamconfigdialog.ui

RESOURCES += qfecamsystemcamera.qrc

TRANSLATIONS= ./translations/de.cam_systemcam.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



