TEMPLATE = lib
CONFIG += plugin


TARGET = stage_pi863
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

QFOUTPUT = ../../output/
DESTDIR = $$QFOUTPUT/plugins


LIBS += -L$$QFOUTPUT -lquickfit3lib

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

TRANSLATIONS= ./translations/de.stage_pi863.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg



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

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

