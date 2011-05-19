TEMPLATE = lib
CONFIG += plugin


TARGET = cam_mystagename
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = ../../output/plugins/

LIBS += -L../../output/ -lquickfit3lib

# Input
HEADERS += qfextensionlinearstage.h \
           ../interfaces/qfextensionlinearstage.h

SOURCES += qfextensionlinearstage.cpp

FORMS =

RESOURCES += qfextensionlinearstage.qrc

TRANSLATIONS= ../../output/assets/translations/de.cam_mystagename.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

