TEMPLATE = app
TARGET = jkfloatedit_test
DEPENDPATH += . \
               ../../ \
               ../../../
INCLUDEPATH += . \
               ../../

DESTDIR = ./

# Input
HEADERS += jkdoubleedit.h \
           testmain.h

SOURCES += jkfloatedit_test.cpp \
           jkdoubleedit.cpp \
           testmain.cpp

FORMS =

RESOURCES +=


QT += gui
CONFIG += console

MOC_DIR = ./.mocs/
UI_DIR = ./.uics/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
