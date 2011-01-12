TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../output

#QMAKE_CXXFLAGS += -fexceptions -msse -msse2 -mfpmath=387,sse -O2
LIBS += -lgsl -lgslcblas -L../lib/ -lquickfit3lib -L../libqf3widgets/ -lquickfit3widgets
# Input
HEADERS += mainwindow.h \
           version.h \
           optionsdialog.h

SOURCES += main.cpp \
           mainwindow.cpp \
           optionsdialog.cpp

FORMS =    optionsdialog.ui \
           about.ui \
           aboutplugins.ui

RESOURCES += quickfit3.qrc


TRANSLATIONS= ../output/translations/de.ts

INCLUDEPATH += ../../../../LIB/trunk/

QT += gui xml
CONFIG += console



MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
