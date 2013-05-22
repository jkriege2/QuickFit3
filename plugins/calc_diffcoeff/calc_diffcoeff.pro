TEMPLATE = lib
CONFIG += plugin

TARGET = calc_diffcoeff
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfediffusioncoefficientcalculator.h \
    dlgcalcdiffcoeff.h \
    ../interfaces/qfextensiontool.h \
    ../interfaces/qfextensionreportingtool.h \
    dlgcomponentinfo.h \
    optionswidget.h

SOURCES += qfediffusioncoefficientcalculator.cpp \
    dlgcalcdiffcoeff.cpp \
    dlgcomponentinfo.cpp \
    optionswidget.cpp

FORMS = \
    dlgcalcdiffcoeff.ui \
    dlgcomponentinfo.ui \
    optionswidget.ui

RESOURCES += qfediffusioncoefficientcalculator.qrc

TRANSLATIONS= ./translations/de.qfe_calc_diffcoeff.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl



