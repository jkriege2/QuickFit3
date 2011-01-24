TEMPLATE = lib
CONFIG += plugin


TARGET = fitalgorithm_levmar
DEPENDPATH += ./
DLLDESTDIR = ../../output/plugins/fitalgorithms
DESTDIR = ./
LIBS += -llevmar -L../../lib/ -lquickfit3lib

# Input
HEADERS += fitalgorithm_levmar.h

SOURCES += fitalgorithm_levmar.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/


QT += core

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

