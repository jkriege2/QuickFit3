TEMPLATE = lib
CONFIG += plugin


TARGET = photoncounts
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfrdrphotoncounts.h \
           qfrdrphotoncounts_data.h \
           qfrdrphotoncounts_dataeditor.h \
    ../interfaces/qfrdrphotoncountsinterface.h

SOURCES += qfrdrphotoncounts.cpp \
           qfrdrphotoncounts_data.cpp \
           qfrdrphotoncounts_dataeditor.cpp

FORMS = 

RESOURCES += qfrdrphotoncounts.qrc

TRANSLATIONS= ./translations/de.photoncounts.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml core
CONFIG += exceptions rtti stl



ASSETSTESTDATA_FILES.files = ./examples/testdata/*.*
ASSETSTESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/testdata/

INSTALLS += ASSETSTESTDATA_FILES

