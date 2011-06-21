TEMPLATE = lib
CONFIG += plugin

QFOUTPUT=../../output
TARGET = target_id
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./
DESTDIR = $${QFOUTPUT}/plugins/

LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += fitalgorithm_inst.h \
           fitalgorithm_instconfig.h

SOURCES += fitalgorithm_inst.cpp \
           fitalgorithm_instconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ../../output/assets/translations/de.target_id.ts

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


QT += core
CONFIG += exceptions rtti stl
MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

