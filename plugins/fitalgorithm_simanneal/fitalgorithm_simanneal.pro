TEMPLATE = lib
CONFIG += plugin


TARGET = fitalgorithm_simanneal
DEPENDPATH += ./
DESTDIR = ../../output/plugins/fitalgorithms/
#DLLDESTDIR = $$DESTDIR
LIBS += -llevmar -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += fitalgorithm_simanneal.h \
           simannealconfig.h

SOURCES += fitalgorithm_simanneal.cpp \
           simannealconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ../../output/translations/de.fitalgorithm_simanneal.ts

INCLUDEPATH += ../../lib/ \
               ../../extlib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += core
CONFIG += exceptions rtti stl
MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

