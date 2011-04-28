TEMPLATE = lib
CONFIG += plugin


TARGET = cam_mystagename
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = ../../output/plugins/extensions/

LIBS += -L../../output/ -lquickfit3lib

# Input
HEADERS += qfextensionlinearstage.h \
           ../interfaces/qfextensionlinearstage.h

SOURCES += qfextensionlinearstage.cpp

FORMS =

RESOURCES += qfextensionlinearstage.qrc

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

