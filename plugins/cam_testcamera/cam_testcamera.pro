TEMPLATE = lib
CONFIG += plugin


TARGET = cam_testcamera

DEFINES += TARGETNAME=$$TARGET

DEPENDPATH += ./
DESTDIR = ../../output/plugins/extensions/

LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += cam_testcamera.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += cam_testcamera.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += cam_testcamera.qrc

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

