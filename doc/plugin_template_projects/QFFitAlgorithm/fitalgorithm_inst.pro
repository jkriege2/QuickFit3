TEMPLATE = lib
CONFIG += plugin


TARGET = target_id
DEPENDPATH += ./
DESTDIR = ../../output/plugins/
#DLLDESTDIR = $$DESTDIR
win32:LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets
unix:LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += fitalgorithm_inst.h \
           fitalgorithm_instconfig.h

SOURCES += fitalgorithm_inst.cpp \
           fitalgorithm_instconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.target_id.ts

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

