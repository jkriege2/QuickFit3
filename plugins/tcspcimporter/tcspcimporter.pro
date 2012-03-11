TEMPLATE = lib
CONFIG += plugin

TARGET = tcspcimporter
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/

include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfetcspcimporter.h \
    qfetcspcimporterthreadprogress.h \
    qfetcspcimporterjobthread.h \
    qfetcspcimporterdialog.h

SOURCES += qfetcspcimporter.cpp \
    qfetcspcimporterthreadprogress.cpp \
    qfetcspcimporterjobthread.cpp \
    qfetcspcimporterdialog.cpp

FORMS = \
    qfetcspcimporterthreadprogress.ui \
    qfetcspcimporterdialog.ui

RESOURCES += qfetcspcimporter.qrc \
    ../../lib/lib.qrc

TRANSLATIONS= ./translations/de.tcspcimporter.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl

ASSETS_TESTDATA.files = ./assets/testdata/*.*
ASSETS_TESTDATA.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/testdata/

INSTALLS += ASSETS_TESTDATA






