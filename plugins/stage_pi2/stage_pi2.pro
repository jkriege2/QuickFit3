TEMPLATE = lib
CONFIG += plugin
TARGET = stage_pi863_2
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += stage_pi2.h \
           ../interfaces/qfextensionlinearstage.h \
           ../../../../../LIB/trunk/jkserialconnection.h \
    pimercury863calibrationdialog.h \
    ../base_classes/qf3comportmanager.h

SOURCES += stage_pi2.cpp \
           ../../../../../LIB/trunk/jkserialconnection.cpp \
    pimercury863calibrationdialog.cpp \
    ../base_classes/qf3comportmanager.cpp

FORMS =

RESOURCES += stage_pi2.qrc

TRANSLATIONS= ./translations/de.stage_pi863_2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg




