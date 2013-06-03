TEMPLATE = lib
CONFIG += plugin


TARGET = rdr_settings
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrsettings.h \
           qfrdrsettings_data.h \
		   qfrdrsettings_dataeditor.h

SOURCES += qfrdrsettings.cpp \
           qfrdrsettings_data.cpp \
		   qfrdrsettings_dataeditor.cpp

FORMS = 

RESOURCES += qfrdrsettings.qrc

TRANSLATIONS= ./translations/de.rdr_settings.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

ASSETS_TESTDATA.files = ./assets/examples/testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/examples/$${TARGET}/testdata/
INSTALLS += ASSETS_TESTDATA

QT += gui xml core
CONFIG += exceptions rtti stl



