TEMPLATE = lib
CONFIG += plugin
TARGET = stage_pi863
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


LIBS += -L$$QFOUTPUT -lquickfit3lib

# Input
HEADERS += stage_pi.h \
           ../interfaces/qfextensionlinearstage.h \
           ../../../../../LIB/trunk/tools.h \
           ../../../../../LIB/trunk/jkserialconnection.h

SOURCES += stage_pi.cpp \
           ../../../../../LIB/trunk/tools.cpp \
           ../../../../../LIB/trunk/jkserialconnection.cpp

FORMS =

RESOURCES += stage_pi.qrc

TRANSLATIONS= ./translations/de.stage_pi863.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg


