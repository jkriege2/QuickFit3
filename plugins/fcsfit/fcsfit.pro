TEMPLATE = lib
CONFIG += plugin


TARGET = qfevalfcsfit
DEPENDPATH += ./
DESTDIR = ../../output/plugins
#DLLDESTDIR = $$DESTDIR
LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += qfpevalfcsfit.h \
           qffcsfitevaluation.h \
           qffcsfitevaluationeditor.h \
           ../interfaces/qfrdrfcsdatainterface.h \
           ../../../../../LIB/trunk/statistics_tools.h

SOURCES += qfpevalfcsfit.cpp \
           qffcsfitevaluation.cpp \
           qffcsfitevaluationeditor.cpp \
           ../../../../../LIB/trunk/statistics_tools.cpp

FORMS =

RESOURCES += qfevalfcsfit.qrc

TRANSLATIONS= ../../output/translations/de.qfevalfcsfit.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg core
CONFIG += exceptions rtti stl
MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

