TEMPLATE = lib
CONFIG += plugin


TARGET = spimb040
DEPENDPATH += ./


include(../plugins.pri)
include(../../extlibs/lmfit.pri)
include(../../extlibs/tiff.pri)

DESTDIR = $$QFOUTPUT/plugins
include(../../libquickfitwidgets.pri)

# Input
HEADERS += spimb040.h \
           qfespimb040mainwindow.h \
           qfespimb040cameraview.h \
           qfespimb040cameraconfig.h \
           qfespimb040samplestageconfig.h \
           ../interfaces/qfextensioncamera.h \
           ../../../../../LIB/trunk/jkimage.h  \
           ../../../../../LIB/trunk/highrestimer.h \
           ../../../../../LIB/trunk/statistics_tools.h \
           qfespimb040imagestackconfigwidget.h \
           ../../../../../LIB/trunk/tinytiffwriter.h \
           ../../../../../LIB/trunk/libtiff_tools.h \
           ../interfaces/qfextensionlinearstage.h \
           ../interfaces/qfextensionparameterdevice.h \
    qfespimb040acquisitionconfigwidget.h \
    qfespimb040opticssetup.h \
    qfespimb040simplecameraconfig.h \
    qfespimb00histogrammaskdialog.h \
    ../../../../../LIB/trunk/qt/qfasttablelabel.h \
    ../../../../../LIB/trunk/histogram.h \
    qfespimb040mainwindow2.h \
    qfespimb040experimentdescription.h

SOURCES += spimb040.cpp \
           qfespimb040mainwindow.cpp \
           qfespimb040cameraview.cpp \
           qfespimb040cameraconfig.cpp \
           qfespimb040samplestageconfig.cpp \
           ../../../../../LIB/trunk/highrestimer.cpp \
           ../../../../../LIB/trunk/statistics_tools.cpp \
           qfespimb040imagestackconfigwidget.cpp \
           ../../../../../LIB/trunk/tinytiffwriter.cpp \
           ../../../../../LIB/trunk/libtiff_tools.cpp \
    qfespimb040acquisitionconfigwidget.cpp \
    qfespimb040opticssetup.cpp \
    qfespimb040simplecameraconfig.cpp \
    qfespimb00histogrammaskdialog.cpp \
    ../../../../../LIB/trunk/qt/qfasttablelabel.cpp \
    qfespimb040mainwindow2.cpp \
    qfespimb040experimentdescription.cpp
FORMS = \
    qfespimb040imagestackconfigwidget.ui \
    qfespimb040acquisitionconfigwidget.ui \
    qfespimb040opticssetup.ui \
    qfespimb00histogrammaskdialog.ui \
    qfespimb040experimentdescription.ui

RESOURCES += spimb040.qrc

TRANSLATIONS= ./translations/de.spimb040.ts

INCLUDEPATH += . \
               ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

QT += gui xml svg opengl

CONFIG += exceptions rtti stl

















