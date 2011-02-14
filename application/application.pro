TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += . \
               ../lib/ \
               ../libqf3widgets/
INCLUDEPATH += . \
               ../lib/ \
               ../libqf3widgets/
DESTDIR = ../output

LIBS += -L../output/ -lquickfit3lib -lquickfit3widgets
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

RC_FILE = quickfit3.rc

QT += gui xml
CONFIG += console



MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
