TEMPLATE = lib
CONFIG += plugin


TARGET = qfrdrtable
DEPENDPATH += ./
DESTDIR = ../../output/plugins
#DLLDESTDIR = $$DESTDIR
LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += dlgcsvparameters.h \
           qfrdrtable.h \
           qfrdrtableeditor.h \
           qftablemodel.h \
           qfprdrtable.h

SOURCES += dlgcsvparameters.cpp \
           qfrdrtable.cpp \
           qfrdrtableeditor.cpp \
           qftablemodel.cpp \
           qfprdrtable.cpp

FORMS = dlg_csvparameters.ui

RESOURCES += qfrdrtable.qrc

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/ \
               ../../../../../LIB/trunk/

QT += gui xml
CONFIG += exceptions rtti stl
MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

