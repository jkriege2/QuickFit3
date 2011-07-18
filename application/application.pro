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

include(../quickfit3.pri)

DESTDIR = $$QFOUTPUT

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






ASSETSSTYLESHEET_FILES.files = ./assets/stylesheets/*.*
ASSETSSTYLESHEET_FILES.path = $${QFOUTPUT}/assets/stylesheets/

INSTALLS += ASSETSSTYLESHEET_FILES
