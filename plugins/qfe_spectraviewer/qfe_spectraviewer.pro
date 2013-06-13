TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_spectraviewer
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)
include(../../libquickfitwidgets.pri)
include(../../extlibs/gsl.pri)
DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfespectraviewer.h \
    qfespectraviewerdialog.h \
    optionswidget.h \
    spectrummanager.h

SOURCES += qfespectraviewer.cpp \
    qfespectraviewerdialog.cpp \
    optionswidget.cpp \
    spectrummanager.cpp

FORMS = \
    qfespectraviewerdialog.ui \
    optionswidget.ui

RESOURCES += qfespectraviewer.qrc

TRANSLATIONS= ./translations/de.qfe_spectraviewer.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



