include(../../../../../SPIM/trunk/software/processing_chain/processing_chain.pri)

QMAKE_CXXFLAGS += -ggdb

TEMPLATE = lib
CONFIG += plugin

debug:QFOUTPUT = ../../output
!debug:QFOUTPUT = ../../output_release

TARGET = cam_rh2v2
DEPENDPATH += ./
DESTDIR = $$QFOUTPUT/plugins/

DEFINES += TARGETNAME=$$TARGET

LIBS += -lusb -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets
win32:LIBS += -lgdi32

# Input
HEADERS += cam_rh2v2.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += cam_rh2v2.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += cam_rh2v2.qrc

TRANSLATIONS= $$QFOUTPUT/assets/translations/de.cam_rh2v2.ts

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

