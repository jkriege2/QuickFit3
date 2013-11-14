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
    spectrummanager.h \
    qfespectraviewerlightsourceeditor.h \
    qfespectraviewerfluorophoreditor.h \
    qfespectraviewerfiltereditor.h \
    qfespectraviewerspilloverdialog.h \
    qfespectraviewerdetectoreditor.h \
    qfespectraviewerfretdialog.h

SOURCES += qfespectraviewer.cpp \
    qfespectraviewerdialog.cpp \
    optionswidget.cpp \
    spectrummanager.cpp \
    qfespectraviewerlightsourceeditor.cpp \
    qfespectraviewerfluorophoreditor.cpp \
    qfespectraviewerfiltereditor.cpp \
    qfespectraviewerspilloverdialog.cpp \
    qfespectraviewerdetectoreditor.cpp \
    qfespectraviewerfretdialog.cpp

FORMS = \
    qfespectraviewerdialog.ui \
    optionswidget.ui \
    qfespectraviewerlightsourceeditor.ui \
    qfespectraviewerfluorophoreditor.ui \
    qfespectraviewerfiltereditor.ui \
    qfespectraviewerspilloverdialog.ui \
    qfespectraviewerdetectoreditor.ui \
    qfespectraviewerfretdialog.ui

RESOURCES += qfespectraviewer.qrc \
    ../../lib/lib.qrc

TRANSLATIONS= ./translations/de.qfe_spectraviewer.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



