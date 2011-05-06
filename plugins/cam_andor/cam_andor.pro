TEMPLATE = lib
CONFIG += plugin


TARGET = cam_andor
DEPENDPATH += ./
DESTDIR = ../../output/plugins/extensions/

DEFINES += TARGETNAME=$$TARGET

LIBS += -lusb -L../../output/ -lquickfit3lib -lquickfit3widgets
win32:LIBS += -lgdi32

# Input
HEADERS += cam_andor.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += cam_andor.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += cam_andor.qrc

TRANSLATIONS= ../../output/translations/de.cam_andor.ts

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

