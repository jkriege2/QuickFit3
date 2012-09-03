TEMPLATE = lib
CONFIG += plugin
TARGET = scanner2000_nicounter
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../extlibs/nidaqmx.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

HEADERS += \
           ../../../../../LIB/trunk/jkserialconnection.h \
           scanner2000nicounter.h \
           scancontroller.h \
           ../../extlibs/NIDAQmx/NIDAQmx.h \
    nidaq6602.h


SOURCES += \
           ../../../../../LIB/trunk/jkserialconnection.cpp \
           scanner2000nicounter.cpp \
           scancontroller.cpp \
    nidaq6602.cpp

FORMS =

RESOURCES += \


TRANSLATIONS= ./translations/de.stage_mc2000.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
