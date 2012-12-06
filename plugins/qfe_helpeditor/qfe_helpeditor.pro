TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_helpeditor
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfehelpeditor.h \
           qfehelpeditorwidget.h \
    replacedialog.h \
    finddialog.h

SOURCES += qfehelpeditor.cpp \
           qfehelpeditorwidget.cpp \
    replacedialog.cpp \
    finddialog.cpp

FORMS =    qfehelpeditorwidget.ui \
    replacedialog.ui \
    finddialog.ui

RESOURCES += qfehelpeditor.qrc

TRANSLATIONS= ./translations/de.qfe_helpeditor.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



