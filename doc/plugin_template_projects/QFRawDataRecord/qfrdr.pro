TEMPLATE = lib
CONFIG += plugin

QFOUTPUT = ../../output/
DESTDIR = $$QFOUTPUT/plugins

TARGET = target_id
DEPENDPATH += ./

LIBS += -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += qfrdr.h \
           qfrdr_data.h \
		   qfrdr_dataeditor.h

SOURCES += qfrdr.cpp \
           qfrdr_data.cpp \
		   qfrdr_dataeditor.cpp

FORMS = 

RESOURCES += qfrdr.qrc

TRANSLATIONS= ./translations/de.target_id.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg
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

POST_TARGETDEPS += install


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

