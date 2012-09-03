TEMPLATE = lib
CONFIG += plugin

TARGET = b040_ffmcontrol
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/
include(../../libquickfitwidgets.pri)



# Input
HEADERS += qfeb040ffmcontrol.h \
    ffmmainwindow.h \
    ffmexpcontrolwidget.h \
    ffmimagewidget.h \
    ffmexpdescriptionwidget.h \
    ../../../../../LIB/trunk/jkserialconnection.h \




SOURCES += qfeb040ffmcontrol.cpp \
    ffmmainwindow.cpp \
    ffmexpcontrolwidget.cpp \
    ffmimagewidget.cpp \
    ffmexpdescriptionwidget.cpp \
    ../../../../../LIB/trunk/jkserialconnection.cpp \



FORMS =

RESOURCES += qfeb040ffmcontrol.qrc

TRANSLATIONS= ./translations/de.b040_ffmcontrol.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/ \


QT += gui xml svg opengl
CONFIG += exceptions rtti stl



