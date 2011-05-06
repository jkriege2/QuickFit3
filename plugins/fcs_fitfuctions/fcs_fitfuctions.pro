TEMPLATE = lib
CONFIG += plugin


TARGET = qffcsfitfuncs
DEPENDPATH += ./
DESTDIR = ../../output/plugins/fitfunctions
#DLLDESTDIR = $$DESTDIR
LIBS +=

# Input
HEADERS += fcs_fitfuctions.h

SOURCES += fcs_fitfuctions.cpp

FORMS =

RESOURCES +=

TRANSLATIONS= ../../output/translations/de.qffcsfitfuncs.ts

INCLUDEPATH += ../../lib/


QT += core
CONFIG += exceptions rtti stl
MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

