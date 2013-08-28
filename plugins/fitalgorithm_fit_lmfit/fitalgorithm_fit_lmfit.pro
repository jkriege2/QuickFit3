TEMPLATE = lib
CONFIG += plugin

TARGET = fit_lmfit
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
include(../plugins.pri)
include(../../extlibs/lmfit.pri)

DESTDIR = $${QFOUTPUT}/plugins/
include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfpfitalgorithmlmfit.h \
		   qffitalgorithmlmfit.h \ 
    qffitalgorithmlmfitconfig.h \
    qffitalgorithmlmfitirls.h \
    qffitalgorithmlmfitbox.h

SOURCES += qfpfitalgorithmlmfit.cpp \
		   qffitalgorithmlmfit.cpp \
    qffitalgorithmlmfitconfig.cpp \
    qffitalgorithmlmfitirls.cpp \
    qffitalgorithmlmfitbox.cpp

FORMS += \
    qffitalgorithmlmfitconfig.ui

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.fit_lmfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

SRC_DISTRIBUTED.files = $$HEADERS \
                        $$SOURCES \
                        $$FORMS \
                        $$RESOURCES \
                        $$TRANSLATIONS \
                        $$GPL3LICENSE


QT += gui xml svg
CONFIG += exceptions rtti stl


