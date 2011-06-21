TEMPLATE = lib
CONFIG += plugin

QFOUTPUT=../../output
TARGET = target_id
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = $${QFOUTPUT}/plugins/

LIBS += -L$${QFOUTPUT} -lquickfit3lib

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


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

