TEMPLATE = app
TARGET = widgets_test
DEFINES += QT_DLL
DESTDIR = ./

LIBS +=

INCLUDEPATH += . \
               ../../

HEADERS     = testmain.h \
			  ../../flowlayout.h \
			  ../../jkstarratingwidget.h \
			  ../../qenhancedlineedit.h \
			  ../../qspecialtoolbutton.h \
			  ../../jkstyledbutton.h \
			  ../../jkcharacterscreenkeyboard.h \
			  ../../qmodernprogresswidget.h \
			  ../../qmoretextobject.h \
    ../../qenhancedtableview.h \
    ../../qenhancedprintpreviewdialog.h

RESOURCES   = ../../qspecialtoolbutton.qrc \
              ../../jkstyledbutton.qrc \
              test.qrc

SOURCES     = test.cpp \
              testmain.cpp  \
			  ../../flowlayout.cpp \
			  ../../jkstarratingwidget.cpp \
			  ../../qenhancedlineedit.cpp \
			  ../../qspecialtoolbutton.cpp \
			  ../../jkstyledbutton.cpp \
			  ../../jkcharacterscreenkeyboard.cpp \
			  ../../qmodernprogresswidget.cpp \
			  ../../qmoretextobject.cpp \
    ../../qenhancedtableview.cpp \
    ../../qenhancedprintpreviewdialog.cpp

FORMS       =

TRANSLATIONS=

QT += gui core svg
CONFIG += console

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
