TEMPLATE = lib
TARGET = quickfit3widgets
DEPENDPATH += .

INCLUDEPATH += ../../../../LIB/trunk/

QT += gui xml svg
DESTDIR = ./
DLLDESTDIR = ../output

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
           qehtreeview.h \
           datacutslider.h \
           ../../../../LIB/trunk/qt/numberedit.h \
           qfhtmlhelpwindow.h \
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
           qehtreeview.cpp \
           datacutslider.cpp \
           ../../../../LIB/trunk/qt/numberedit.cpp \
           qfhtmlhelpwindow.cpp \
           qffitparameterwidget.cpp \
           qfdlgcsvparameters.cpp

FORMS += qfdlg_csvparameters.ui

RESOURCES += ../../../../LIB/trunk/qt/jkqtpbaseplotter.qrc

TRANSLATIONS= ../output/translations/de.ts

INCLUDEPATH += ../../../../LIB/trunk/ \
               ../../../../LIB/trunk/qt/


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
