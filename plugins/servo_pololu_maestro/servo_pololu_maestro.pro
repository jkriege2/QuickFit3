TEMPLATE = lib
CONFIG += plugin
TARGET = servo_pololu_maestro
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += servo_pololu_maestro.h \
           ../interfaces/qfextensionshutter.h \
           ../../../../../LIB/trunk/jkserialconnection.h \
    ../base_classes/qfbinaryserialprotocolhandler.h \
    ../base_classes/qf3comportmanager.h \
    dlgpololumaestrotester.h \
    pololu_tools.h \
    ../interfaces/qfextensionfilterchanger.h

SOURCES += servo_pololu_maestro.cpp \
           ../../../../../LIB/trunk/jkserialconnection.cpp \
    ../base_classes/qfbinaryserialprotocolhandler.cpp \
    ../base_classes/qf3comportmanager.cpp \
    dlgpololumaestrotester.cpp \
    pololu_tools.cpp

FORMS = \
    dlgpololumaestrotester.ui

RESOURCES += servo_pololu_maestro.qrc

TRANSLATIONS= ./translations/de.servo_pololu_maestro.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg






