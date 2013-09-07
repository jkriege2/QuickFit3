TEMPLATE = lib
CONFIG += plugin
TARGET = meas_b040resheater
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += meas_b040resheater.h \
           ../../../../../LIB/trunk/jkserialconnection.h \
    ../base_classes/qf3simpleb040serialprotocolhandler.h \
    ../base_classes/qf3comportmanager.h \
    ../interfaces/qfextensionmeasurementdevice.h

SOURCES += meas_b040resheater.cpp \
           ../../../../../LIB/trunk/jkserialconnection.cpp \
    ../base_classes/qf3simpleb040serialprotocolhandler.cpp \
    ../base_classes/qf3comportmanager.cpp

FORMS =

RESOURCES += meas_b040resheater.qrc

TRANSLATIONS= ./translations/de.meas_b040resheater.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg






