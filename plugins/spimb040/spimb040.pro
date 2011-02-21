TEMPLATE = lib
CONFIG += plugin


TARGET = spimb040
DEPENDPATH += ./
DESTDIR = ../../output/plugins/extensions/
#DLLDESTDIR = $$DESTDIR
# ./
LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += spimb040.h

SOURCES += spimb040.cpp

FORMS = 

RESOURCES += spimb040.qrc

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

