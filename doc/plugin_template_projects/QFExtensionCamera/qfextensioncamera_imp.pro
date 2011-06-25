TEMPLATE = lib
CONFIG += plugin

TARGET = target_id
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/

LIBS += -L$${QFOUTPUT} -lquickfit3lib

# Input
HEADERS += qfextensioncamera_imp.h \
           ../interfaces/qfextensioncamera.h

SOURCES += qfextensioncamera_imp.cpp

FORMS =

RESOURCES += qfextensioncamera_imp.qrc

TRANSLATIONS= ./translations/de.target_id.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



