TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_dataexportbasics
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfe_dataexportbasics.h \
    qfedataxlslibexport.h

SOURCES += qfe_dataexportbasics.cpp \
    qfedataxlslibexport.cpp

FORMS =

RESOURCES += qfe_dataexportbasics.qrc

TRANSLATIONS= ./translations/de.qfe_dataexportbasics.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

include(xlslib.pri)

QT += gui xml svg
CONFIG += exceptions rtti stl



SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE \
                        xlslib-package-2.4.0.zip

OTHER_FILES += \
    xlslib.pri
