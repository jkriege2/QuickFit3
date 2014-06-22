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
    cairo4qt/src/qpaintengine_cairo_p.h \
    cairo4qt/src/qpaintengine_p.h \
    cairo4qt/src/qt_x11_p.h \
    cairo4qt/src/qcairopaintdevice.h

SOURCES += qfe_plotterexportercairo.cpp \
    jkqtcairoengineadapter.cpp \ 
    cairo4qt/src/brushpattern.cpp \
    cairo4qt/src/qpaintengine_cairo.cpp \
    cairo4qt/src/qcairopaintdevice.cpp



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
                        $$GPL3LICENSE \
                        cairo4qt-0.9.1.tar.gz
