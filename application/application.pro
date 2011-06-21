TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += . \
               ../lib/ \
               ../libqf3widgets/
INCLUDEPATH += . \
               ../lib/ \
               ../libqf3widgets/ \
               ../../../../LIB/trunk/ \
               ../../../../LIB/trunk/qt
DESTDIR = ../output
QFOUTPUT = $$DESTDIR

QMAKE_RPATHDIR += $$DESTDIR

LIBS += -L$$DESTDIR -lquickfit3lib -lquickfit3widgets
# Input
HEADERS += mainwindow.h \
           version.h \
           optionsdialog.h

SOURCES += main.cpp \
           mainwindow.cpp \
           optionsdialog.cpp

FORMS =    optionsdialog.ui \
           about.ui \
           aboutplugins.ui

RESOURCES += quickfit3.qrc


TRANSLATIONS= ./translations/de.ts

INCLUDEPATH += ../../../../LIB/trunk/

RC_FILE = quickfit3.rc

QT += gui xml
CONFIG += console exceptions rtti stl

# DEFINITION FOR CImg Library
include( ../extlibs/cimg.pri )
PRECOMPILED_HEADER += ../extlibs/cimg.h






HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/help/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/help/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/

ASSETSTESTDATA_FILES.files = ./assets/testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/assets/testdata/

ASSETSSTYLESHEET_FILES.files = ./assets/stylesheets/*.*
ASSETSSTYLESHEET_FILES.path = $${QFOUTPUT}/assets/stylesheets/

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES ASSETS_TRANSLATIONS ASSETSSTYLESHEET_FILES ASSETSTESTDATA_FILES

POST_TARGETDEPS += install

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

