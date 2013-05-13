TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_defaultmathparserextensions
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfedefaultmathparserextensions.h \
    qfedefaultmathparserextensionfunctions.h \
    ../../../../../LIB/trunk/statistics_tools.h

SOURCES += qfedefaultmathparserextensions.cpp \
    qfedefaultmathparserextensionfunctions.cpp \
    ../../../../../LIB/trunk/statistics_tools.cpp

FORMS =

RESOURCES += qfedefaultmathparserextensions.qrc

TRANSLATIONS= ./translations/de.qfe_defaultmathparserextensions.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

HELPREF_FILES.files = ./help/parserreference/*.*
HELPREF_FILES.path = $${QFOUTPUT}/assets/help/parserreference/

HELPREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPREFPIC_FILES.path = $${QFOUTPUT}/assets/help/parserreference/pic/

INSTALLS += ASSETSSTYLESHEET_FILES HELPREF_FILES HELPREFPIC_FILES ICON_FILES


QT += gui xml svg
CONFIG += exceptions rtti stl



