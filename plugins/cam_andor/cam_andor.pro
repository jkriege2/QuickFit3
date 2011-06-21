TEMPLATE = lib
CONFIG += plugin

QFOUTPUT = ../../output/
DESTDIR = $$QFOUTPUT/plugins
TARGET = cam_andor
DEPENDPATH += ./


DEFINES += TARGETNAME=$$TARGET

win32 {
  LIBS+= -L../../extlibs/andor_win32
  INCLUDEPATH += ../../extlibs/andor_win32
}

LIBS += -landor -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += cam_andor.h \
           camandoracquisitionthread.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += cam_andor.cpp \
           camandoracquisitionthread.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += cam_andor.qrc

TRANSLATIONS= ./translations/de.cam_andor.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/




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

