#-------------------------------------------------
#
# Project created by QtCreator 2013-09-15T14:01:46
#
#-------------------------------------------------

QT       += core gui opengl

CONFIG +=

include(../../qfvisualize3dshaders.pri)
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
    message("Qt Major Version >5, using special Qt5.x include syntax for widgets")
    win32{
        !contains(LIBS, -lopengl32): LIBS += -lopengl32
        !contains(LIBS, -lglu32): LIBS += -lglu32
    }
    greaterThan(QT_MINOR_VERSION, 3) {
        DEFINES += QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    }
}
TARGET = visualize3DOpenGL
TEMPLATE = app

DEFINES += SAVE_CONFIG_VIZ3D NO_JKQTPLOTTER


INCLUDEPATH +=  ../../../ \
                ../../

SOURCES += maingl.cpp\
    ../../../statistics_tools.cpp \
    ../../qfluteditor.cpp \
    ../../jkqtpimagetools.cpp \
    ../../qfvisualize3dglwidget.cpp \
    ../../../highrestimer.cpp \
    ../../colorcombobox.cpp \ 
    ../../qfvisualize3dgleditor.cpp

HEADERS  += ../../../statistics_tools.h \
    ../../qfluteditor.h \
    ../../jkqtpimagetools.h \
    ../../qfvisualize3dglwidget.h \
    ../../../ticktock.h \
    ../../../highrestimer.h \
    ../../colorcombobox.h \ 
    ../../qfvisualize3dgleditor.h

FORMS    +=  ../../qfvisualize3dgleditor.ui

RESOURCES += \
    visualize3D.qrc \
    ../../qfvisualize3d.qrc

UI_DIR = .uics
OBJECTS_DIR = .objs
MOC_DIR = .mocs
RCC_DIR = .rccs
DESTDIR = ./
