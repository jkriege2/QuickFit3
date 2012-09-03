TEMPLATE = lib
CONFIG += plugin
TARGET = stage_mc2000
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += \
           ../interfaces/qfextensionlinearstage.h \
           ../../../../../LIB/trunk/jkserialconnection.h \
    multicontrol_stage.h \
    mcstagecontroller.h \
    ../b040_ffmcontrol/ffmmainlogfile.h \
    multicontrol_settingsdialog.h

SOURCES += \
           ../../../../../LIB/trunk/jkserialconnection.cpp \
    multicontrol_stage.cpp \
    mcstagecontroller.cpp \
    ../b040_ffmcontrol/ffmmainlogfile.cpp \
    multicontrol_settingsdialog.cpp

FORMS =

RESOURCES += \
    stage_mc2000.qrc

TRANSLATIONS= ./translations/de.stage_mc2000.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg




