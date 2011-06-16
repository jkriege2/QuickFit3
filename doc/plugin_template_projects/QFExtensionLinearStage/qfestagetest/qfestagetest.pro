TEMPLATE = lib
CONFIG += plugin


TARGET = stage_test
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = ../../output/plugins/

LIBS += -L../../output/ -lquickfit3lib

# Input
HEADERS += qfestagetest.h \
           ../interfaces/qfextensionlinearstage.h

SOURCES += qfestagetest.cpp

FORMS =

RESOURCES += qfestagetest.qrc

TRANSLATIONS= ../../output/assets/translations/de.stage_test.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

