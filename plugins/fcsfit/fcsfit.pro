TEMPLATE = lib
CONFIG += plugin


TARGET = qfevalfcsfit
DEPENDPATH += ./
DLLDESTDIR = ../../output/plugins
DESTDIR = ./
LIBS += -lgsl -lgslcblas -L../../lib/ -lquickfit3lib -L../../libqf3widgets/ -lquickfit3widgets
# -L../fcs/ -lqfrdrfcs

# Input
HEADERS += qfpevalfcsfit.h \
           qffcsfitevaluation.h \
           qffcsfitevaluationeditor.h \
           ../fcs/qfrdrfcsdatainterface.h

SOURCES += qfpevalfcsfit.cpp \
           qffcsfitevaluation.cpp \
           qffcsfitevaluationeditor.cpp

FORMS =

RESOURCES += qfevalfcsfit.qrc

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg core

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

