TEMPLATE = lib
CONFIG += plugin
TARGET = alv_autocorrelator5000
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)


DESTDIR = $$QFOUTPUT/plugins


include(../../libquickfitwidgets.pri)

# Input
HEADERS += \
     alv_autocorrelator5000.h \
    alvsma5edataout.h \
    ../interfaces/qfextensionacquisition.h \
    alvsma5edatain.h \
    alvsettingsdialog.h

SOURCES += \
    alv_autocorrelator5000.cpp \
    alvsma5edataout.cpp \
    alvsma5edatain.cpp \
    alvsettingsdialog.cpp

FORMS =

RESOURCES += \


TRANSLATIONS= ./translations/de.alv_autocorrelator5000.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg




