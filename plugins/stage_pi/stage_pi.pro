TEMPLATE = lib
CONFIG += plugin


TARGET = stage_pi863
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = ../../output/plugins/extensions/

LIBS += -L../../output/ -lquickfit3lib

# Input
HEADERS += stage_pi.h \
           ../interfaces/qfextensionlinearstage.h \
           ../../../../../LIB/trunk/tools.h \
           ../../../../../LIB/trunk/jkserialconnection.h

SOURCES += stage_pi.cpp \
           ../../../../../LIB/trunk/tools.cpp \
           ../../../../../LIB/trunk/jkserialconnection.cpp

FORMS =

RESOURCES += stage_pi.qrc

TRANSLATIONS= ../../output/translations/de.stage_pi863.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

