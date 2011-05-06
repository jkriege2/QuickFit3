TEMPLATE = lib
CONFIG += plugin


TARGET = cam_radhard2
DEPENDPATH += ./
DESTDIR = ../../output/plugins/extensions/

DEFINES += TARGETNAME=$$TARGET

LIBS += -lusb -L../../output/ -lquickfit3lib -lquickfit3widgets
win32:LIBS += -lgdi32

# Input
HEADERS += cam_radhard2.h \
           radhard2lib/radhard2.h \
           radhard2lib/radhard2lib.h \
           radhard2lib/radhard2flash.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += cam_radhard2.cpp \
           radhard2lib/radhard2.cpp \
           radhard2lib/radhard2flash.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += cam_radhard2.qrc

TRANSLATIONS= ../../output/translations/de.cam_radhard2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
CONFIG += exceptions rtti stl
MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

