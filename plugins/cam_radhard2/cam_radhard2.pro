TEMPLATE = lib
CONFIG += plugin

QFOUTPUT = ../../output/
DESTDIR = $$QFOUTPUT/plugins

TARGET = cam_radhard2
DEPENDPATH += ./


DEFINES += TARGETNAME=$$TARGET

LIBS += -lusb -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets
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

TRANSLATIONS= ./translations/de.cam_radhard2.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QMAKE_MKDIR = mkdir -p

HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES ASSETS_TRANSLATIONS

POST_TARGETDEPS += install

QT += gui xml svg
CONFIG += exceptions rtti stl
MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

