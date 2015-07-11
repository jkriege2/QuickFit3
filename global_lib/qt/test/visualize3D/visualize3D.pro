#-------------------------------------------------
#
# Project created by QtCreator 2013-09-15T14:01:46
#
#-------------------------------------------------

QT       += core gui

CONFIG += qt3d


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visualize3D
TEMPLATE = app
INCLUDEPATH +=  ../../../ \
                ../../


SOURCES += main.cpp\
        mainwindow.cpp \
    qfvisualize3dwidget.cpp \
    ../../../statistics_tools.cpp \
    qfluteditor.cpp \
    ../../jkqtpimagetools.cpp \
    qfvisualize3dglwidget.cpp \
    ../../../highrestimer.cpp \
    ../../colorcombobox.cpp \
    qfvisualize3dwidget.cpp

HEADERS  += mainwindow.h \
    ../../../statistics_tools.h \
    qfluteditor.h \
    ../../jkqtpimagetools.h \
    qfvisualize3dglwidget.h \
    ../../../ticktock.h \
    ../../../highrestimer.h \
    ../../colorcombobox.h \
    qfvisualize3dwidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    visualize3D.qrc
