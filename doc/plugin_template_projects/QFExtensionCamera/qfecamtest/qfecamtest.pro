TEMPLATE = lib
CONFIG += plugin


TARGET = cam_test
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = ../../output/plugins/

LIBS += -L../../output/ -lquickfit3lib

# Input
HEADERS += qfecamtest.h \
           ../interfaces/qfextensioncamera.h

SOURCES += qfecamtest.cpp

FORMS =

RESOURCES += qfecamtest.qrc

TRANSLATIONS= ../../output/assets/translations/de.cam_test.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

