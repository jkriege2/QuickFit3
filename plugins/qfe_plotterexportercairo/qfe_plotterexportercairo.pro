TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_plotterexportercairo
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../libquickfitwidgets.pri)
include(../../extlibs/cairo.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfe_plotterexportercairo.h \
    jkqtcairoengineadapter.h \
    qcairopaintdevice.h \
    qcairopaintengine.h
SOURCES += qfe_plotterexportercairo.cpp \
    jkqtcairoengineadapter.cpp \
    qcairopaintdevice.cpp \
    qcairopaintengine.cpp



FORMS =

RESOURCES += qfe_plotterexportercairo.qrc

TRANSLATIONS= ./translations/de.qfe_plotterexportercairo.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl




SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE
