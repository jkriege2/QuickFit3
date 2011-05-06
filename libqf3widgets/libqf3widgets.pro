TEMPLATE = lib
TARGET = quickfit3widgets
DEPENDPATH += .

INCLUDEPATH += ../../../../LIB/trunk/ \
               . \
               ../lib/

LIBS += -L../output/ -lquickfit3lib

QT += gui xml svg opengl
DESTDIR = ../output/
#DLLDESTDIR = $$DESTDIR

# Input
HEADERS += ../../../../LIB/trunk/qt/jkqtpbaseplotter.h \
           ../../../../LIB/trunk/qt/jkqtpdatastorage.h \
           ../../../../LIB/trunk/qt/jkqtpelements.h \
           ../../../../LIB/trunk/qt/jkqtpgeoelements.h \
           ../../../../LIB/trunk/qt/jkqtmathtext.h \
           ../../../../LIB/trunk/qt/jkqtpbaseelements.h \
           ../../../../LIB/trunk/qt/jkqtplotter.h \
           ../../../../LIB/trunk/qt/jkqtptools.h \
           ../../../../LIB/trunk/qt/jkqttools.h \
           ../../../../LIB/trunk/qt/jkqtfastplotter.h \
           ../../../../LIB/trunk/qt/qtlogfile.h \
           ../../../../LIB/trunk/qt/qtshortcutwidgets.h \
           ../../../../LIB/trunk/tools.h \
           ../../../../LIB/trunk/datatable2.h \
           ../../../../LIB/trunk/qt/numberedit.h \
           ../../../../LIB/trunk/qt/jkdoubleedit.h \
           ../../../../LIB/trunk/qt/jkverticalscrollarea.h \
           ../../../../LIB/trunk/qt/colorcombobox.h \
           ../../../../LIB/trunk/qt/jkstyledbutton.h \
           ../../../../LIB/trunk/qt/qenhancedlineedit.h \
           ../../../../LIB/trunk/qt/qspecialtoolbutton.h \
           qehtreeview.h \
           datacutslider.h \
           qffitparameterbasicinterface.h \
           qffitparameterwidget.h \
           qfdlgcsvparameters.h


SOURCES += ../../../../LIB/trunk/qt/jkqtpbaseplotter.cpp \
           ../../../../LIB/trunk/qt/jkqtpdatastorage.cpp \
           ../../../../LIB/trunk/qt/jkqtpelements.cpp \
           ../../../../LIB/trunk/qt/jkqtpgeoelements.cpp \
           ../../../../LIB/trunk/qt/jkqtmathtext.cpp \
           ../../../../LIB/trunk/qt/jkqtpbaseelements.cpp \
           ../../../../LIB/trunk/qt/jkqtplotter.cpp \
           ../../../../LIB/trunk/qt/jkqtptools.cpp \
           ../../../../LIB/trunk/qt/jkqttools.cpp \
           ../../../../LIB/trunk/qt/qtlogfile.cpp \
           ../../../../LIB/trunk/qt/jkqtfastplotter.cpp \
           ../../../../LIB/trunk/qt/qtshortcutwidgets.cpp \
           ../../../../LIB/trunk/tools.cpp \
           ../../../../LIB/trunk/datatable2.cpp \
           ../../../../LIB/trunk/qt/numberedit.cpp \
           ../../../../LIB/trunk/qt/jkdoubleedit.cpp \
           ../../../../LIB/trunk/qt/jkverticalscrollarea.cpp \
           ../../../../LIB/trunk/qt/colorcombobox.cpp \
           ../../../../LIB/trunk/qt/jkstyledbutton.cpp \
           ../../../../LIB/trunk/qt/qenhancedlineedit.cpp \
           ../../../../LIB/trunk/qt/qspecialtoolbutton.cpp \
           qehtreeview.cpp \
           datacutslider.cpp \
           qffitparameterwidget.cpp \
           qfdlgcsvparameters.cpp

FORMS += qfdlg_csvparameters.ui

RESOURCES += ../../../../LIB/trunk/qt/jkqtpbaseplotter.qrc \
             ../../../../LIB/trunk/qt/qspecialtoolbutton.qrc \
             ../../../../LIB/trunk/qt/jkstyledbutton.qrc

TRANSLATIONS= ../output/translations/de.quickfit3widgets.ts

INCLUDEPATH += ../../../../LIB/trunk/ \
               ../../../../LIB/trunk/qt/

CONFIG += exceptions rtti stl

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
