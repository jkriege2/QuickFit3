include(../../../../../SPIM/trunk/software/processing_chain/processing_chain.pri)

QMAKE_CXXFLAGS += -ggdb

TEMPLATE = lib
CONFIG += plugin

TARGET = cam_rh2v2
DEPENDPATH += ./


include(../plugins.pri)
include(../../extlibs/usb.pri)

DESTDIR = $$QFOUTPUT/plugins


DEFINES += TARGETNAME=$$TARGET

LIBS += -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += cam_rh2v2.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += cam_rh2v2.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += cam_rh2v2.qrc

TRANSLATIONS= ./translations/de.cam_rh2v2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl


