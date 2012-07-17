include(../../../../../SPIM/trunk/software/processing_chain/processing_chain.pri)

QMAKE_CXXFLAGS += -ggdb

TEMPLATE = lib
CONFIG += plugin

TARGET = cam_rh2v2
DEPENDPATH += ./


include(../plugins.pri)
include(../../extlibs/usb1.0.pri)

DESTDIR = $$QFOUTPUT/plugins


DEFINES += TARGETNAME=$$TARGET

include(../../libquickfitwidgets.pri)

# Input
HEADERS += cam_rh2v2.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h \
    qfradhard2flashtool.h \
    ../base_classes/radhard2flash.h \
    cam_rh2v2_cfgdlg.h \
    cam_rh2v2_cordlg.h

SOURCES += cam_rh2v2.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp \
    qfradhard2flashtool.cpp \
    ../base_classes/radhard2flash.cpp \
    cam_rh2v2_cfgdlg.cpp \
    cam_rh2v2_cordlg.cpp

FORMS = \
    qfradhard2flashtool.ui \
    cam_rh2v2_cfgdlg.ui \
    cam_rh2v2_cordlg.ui

RESOURCES += cam_rh2v2.qrc

TRANSLATIONS= ./translations/de.cam_rh2v2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl

OTHER_FILES += \
    cam_rh2v2.cor.ini \
    cam_rh2v2.raw.ini
