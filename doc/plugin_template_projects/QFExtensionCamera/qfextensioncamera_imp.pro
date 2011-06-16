TEMPLATE = lib
CONFIG += plugin


TARGET = cam_mycamname
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = ../../output/plugins/

LIBS += -L../../output/ -lquickfit3lib

# Input
HEADERS += qfextensioncamera_imp.h \
           ../interfaces/qfextensioncamera.h

SOURCES += qfextensioncamera_imp.cpp

FORMS =

RESOURCES += qfextensioncamera_imp.qrc

TRANSLATIONS= ../../output/assets/translations/de.cam_mycamname.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

