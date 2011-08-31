TEMPLATE = lib
CONFIG += plugin

TARGET = fit_lmfit
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
include(../plugins.pri)
include(../../extlibs/lmfit.pri)

DESTDIR = $${QFOUTPUT}/plugins/

LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += qfpfitalgorithmlmfit.h \
		   qffitalgorithmlmfit.h \ 
    qffitalgorithmlmfitconfig.h

SOURCES += qfpfitalgorithmlmfit.cpp \
		   qffitalgorithmlmfit.cpp \
    qffitalgorithmlmfitconfig.cpp

FORMS += \
    qffitalgorithmlmfitconfig.ui

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.fit_lmfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += gui xml svg
CONFIG += exceptions rtti stl
