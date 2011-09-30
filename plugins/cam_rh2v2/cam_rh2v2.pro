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

include(../../libquickfitwidgets.pri)

# Input
HEADERS += cam_rh2v2.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h \
    qfradhard2flashtool.h

SOURCES += cam_rh2v2.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp \
    qfradhard2flashtool.cpp

FORMS = \
    qfradhard2flashtool.ui

RESOURCES += cam_rh2v2.qrc \
    cam_radhard2.qrc

TRANSLATIONS= ./translations/de.cam_rh2v2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl






