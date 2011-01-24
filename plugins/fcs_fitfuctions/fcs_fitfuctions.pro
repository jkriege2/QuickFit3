TEMPLATE = lib
CONFIG += plugin


TARGET = qffcsfitfuncs
DEPENDPATH += ./
DLLDESTDIR = ../../output/plugins/fitfunctions
DESTDIR = ./
LIBS +=

# Input
HEADERS += fcs_fitfuctions.h

SOURCES += fcs_fitfuctions.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/


QT += core

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

