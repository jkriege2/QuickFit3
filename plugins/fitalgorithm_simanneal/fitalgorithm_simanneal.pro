TEMPLATE = lib
CONFIG += plugin

QFOUTPUT = ../../output/
DESTDIR = $$QFOUTPUT/plugins


TARGET = fitalgorithm_simanneal
DEPENDPATH += ./

LIBS += -llevmar -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += fitalgorithm_simanneal.h \
           simannealconfig.h

SOURCES += fitalgorithm_simanneal.cpp \
           simannealconfig.cpp

FORMS +=

RESOURCES +=

TRANSLATIONS= ./translations/de.fitalgorithm_simanneal.ts

INCLUDEPATH += ../../lib/ \
               ../../extlib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/


QT += core
CONFIG += exceptions rtti stl



HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES ASSETS_TRANSLATIONS

#POST_TARGETDEPS += install

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

