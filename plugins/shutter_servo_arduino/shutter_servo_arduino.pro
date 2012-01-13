TEMPLATE = lib
CONFIG += plugin
TARGET = shutter_servo_arduino
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += shutter_servo_arduino.h \
           ../interfaces/qfextensionshutter.h \
           ../../../../../LIB/trunk/jkserialconnection.h \
    ../base_classes/qf3simpleb040serialprotocolhandler.h \
    ../base_classes/qf3comportmanager.h

SOURCES += shutter_servo_arduino.cpp \
           ../../../../../LIB/trunk/jkserialconnection.cpp \
    ../base_classes/qf3simpleb040serialprotocolhandler.cpp \
    ../base_classes/qf3comportmanager.cpp

FORMS =

RESOURCES += shutter_servo_arduino.qrc

TRANSLATIONS= ./translations/de.shutter_servo_arduino.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg






