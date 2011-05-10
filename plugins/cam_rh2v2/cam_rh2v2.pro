include(../../../../../SPIM/trunk/software/processing_chain/processing_chain.pri)

QMAKE_CXXFLAGS += -ggdb 

TEMPLATE = lib
CONFIG += plugin


TARGET = cam_rh2v2
DEPENDPATH += ./
DESTDIR = ../../output/plugins/extensions/

DEFINES += TARGETNAME=$$TARGET

LIBS += -lusb -L../../output/ -lquickfit3lib -lquickfit3widgets
win32:LIBS += -lgdi32

# Input
HEADERS += cam_rh2v2.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h

SOURCES += cam_rh2v2.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp

FORMS =

RESOURCES += cam_rh2v2.qrc

TRANSLATIONS= ../../output/translations/de.ts

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

