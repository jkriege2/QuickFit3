TEMPLATE = lib
CONFIG += plugin


TARGET = qfrdrtable
DEPENDPATH += ./
DLLDESTDIR = ../../output/plugins
DESTDIR = ./
LIBS += -L../../lib/ -lquickfit3lib

# Input
HEADERS += dlgcsvparameters.h \
           qfrdrtable.h \
           qfrdrtableeditor.h \
           qftablemodel.h \
           ../../../../../LIB/trunk/tools.h \
           ../../../../../LIB/trunk/datatable2.h \
           qfprdrtable.h

SOURCES += dlgcsvparameters.cpp \
           qfrdrtable.cpp \
           qfrdrtableeditor.cpp \
           qftablemodel.cpp\
           ../../../../../LIB/trunk/tools.cpp \
           ../../../../../LIB/trunk/datatable2.cpp \
           qfprdrtable.cpp

FORMS = dlg_csvparameters.ui

RESOURCES += qfrdrtable.qrc

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/ \
               ../../../../../LIB/trunk/

QT += gui xml

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

