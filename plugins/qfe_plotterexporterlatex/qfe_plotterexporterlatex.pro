TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_plotterexporterlatex
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfeplotterexporterlatex.h \
    jkqtplatexengineadapter.h \
    QTeXEngine/src/QTeXEngine.h

SOURCES += qfeplotterexporterlatex.cpp \
    jkqtplatexengineadapter.cpp \
    QTeXEngine/src/QTeXPaintEngine.cpp \
    QTeXEngine/src/QTeXPaintDevice.cpp

FORMS =

RESOURCES += qfeplotterexporterlatex.qrc

TRANSLATIONS= ./translations/de.qfe_plotterexporterlatex.ts

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
                        QTeXEngine-0.3-opensource.zip
