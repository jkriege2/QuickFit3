TEMPLATE = lib
CONFIG += plugin


TARGET = target_id
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../plugins.pri)

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

QT += gui xml svg
CONFIG += exceptions rtti stl



