TEMPLATE = lib
CONFIG += plugin

TARGET = cam_andor
DEPENDPATH += ./

include(../plugins.pri)
include(../../extlibs/andor.pri)

DESTDIR = $$QFOUTPUT/plugins


DEFINES += TARGETNAME=$$TARGET


LIBS += -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += cam_andor.h \
           camandoracquisitionthread.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/highrestimer.h \
           andorglobalcamerasettingswidget.h \
           andorsettingsdialog.h \
           andortools.h \
           ../../../../../LIB/trunk/tinytiffwriter.h

SOURCES += cam_andor.cpp \
           camandoracquisitionthread.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp \
           andorglobalcamerasettingswidget.cpp \
           andorsettingsdialog.cpp \
           andortools.cpp \
           ../../../../../LIB/trunk/tinytiffwriter.cpp

FORMS = \
    andorglobalcamerasettingswidget.ui \
    andorsettingsdialog.ui

RESOURCES += cam_andor.qrc

TRANSLATIONS= ./translations/de.cam_andor.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/ \
               ../../../../../../LIB/trunk/qt/





QT += gui xml svg opengl
CONFIG += exceptions rtti stl
