TEMPLATE = lib
CONFIG += plugin


TARGET = target_id
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = ../../output/plugins/

LIBS += -L../../output/ -lquickfit3lib

# Input
HEADERS += qfextensionlinearstage_imp.h \
           ../interfaces/qfextensionlinearstage.h

SOURCES += qfextensionlinearstage_imp.cpp

FORMS =

RESOURCES += qfextensionlinearstage_imp.qrc

TRANSLATIONS= ../../output/assets/translations/de.target_id.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

