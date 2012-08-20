TEMPLATE = lib
CONFIG += plugin


TARGET = number_and_brightness
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

include(../../extlibs/zlib.pri)
include(../../extlibs/tiff.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrnumberandbrightness.h \
           qfrdrnumberandbrightness_data.h \
		   qfrdrnumberandbrightness_dataeditor.h

SOURCES += qfrdrnumberandbrightness.cpp \
           qfrdrnumberandbrightness_data.cpp \
		   qfrdrnumberandbrightness_dataeditor.cpp

FORMS = 

RESOURCES += numberandbrightness.qrc

TRANSLATIONS= ./translations/de.number_and_brightness.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl



