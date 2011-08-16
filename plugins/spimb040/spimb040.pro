TEMPLATE = lib
CONFIG += plugin


TARGET = spimb040
DEPENDPATH += ./


include(../plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
LIBS += -L../../extlibs/lmfit/ -llmmin -L../../extlibs/libtiff/ -ltiff -L$$QFOUTPUT -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += spimb040.h \
           qfespimb040mainwindow.h \
           qfespimb040cameraview.h \
           qfespimb040cameraconfig.h \
           qfespimb040samplestageconfig.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/jkimage.h  \
           ../../../../../LIB/trunk/highrestimer.h \
           qfcameracombobox.h \
           qfstagecombobox.h \
           qfcameraconfigcombobox.h \
           ../../../../../LIB/trunk/statistics_tools.h \
    objectives.h

SOURCES += spimb040.cpp \
           qfespimb040mainwindow.cpp \
           qfespimb040cameraview.cpp \
           qfespimb040cameraconfig.cpp \
           qfespimb040samplestageconfig.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp \
           qfcameracombobox.cpp \
           qfstagecombobox.cpp \
           qfcameraconfigcombobox.cpp \
           ../../../../../LIB/trunk/statistics_tools.cpp \
    objectives.cpp

FORMS = \
    qfespimb040objectiveeditor.ui

RESOURCES += spimb040.qrc

TRANSLATIONS= ./translations/de.spimb040.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/ \
               ../../extlibs/libtiff/

QT += gui xml svg opengl

CONFIG += exceptions rtti stl
