TEMPLATE = lib
CONFIG += plugin
TARGET = fitalgorithm_levmar
DEPENDPATH += ./

include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins

win32:LIBS += -llevmar -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets
unix:LIBS += -llapack -lblas -llevmar -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += fitalgorithm_levmar.h \
           levmarconfig.h

SOURCES += fitalgorithm_levmar.cpp \
           levmarconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ./translations/de.fitalgorithm_levmar.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += core
CONFIG += exceptions rtti stl


