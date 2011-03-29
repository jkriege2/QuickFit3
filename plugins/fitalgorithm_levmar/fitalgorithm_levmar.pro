TEMPLATE = lib
CONFIG += plugin


TARGET = fitalgorithm_levmar
DEPENDPATH += ./
DESTDIR = ../../output/plugins/fitalgorithms/
#DLLDESTDIR = $$DESTDIR
win32:LIBS += -llevmar -L../../output/ -lquickfit3lib -lquickfit3widgets
unix:LIBS += -llapack -lblas -llevmar -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += fitalgorithm_levmar.h \
           levmarconfig.h

SOURCES += fitalgorithm_levmar.cpp \
           levmarconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += core
CONFIG += exceptions rtti stl
MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

