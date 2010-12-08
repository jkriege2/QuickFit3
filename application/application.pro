TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = ../output

#QMAKE_CXXFLAGS += -fexceptions -msse -msse2 -mfpmath=387,sse -O2
LIBS += -lgsl -lgslcblas -L../lib/ -lquickfit3lib
# Input
HEADERS += mainwindow.h \
           version.h \
           ../../../../LIB/trunk/qt/jkqtpbaseplotter.h \
           ../../../../LIB/trunk/qt/jkqtpdatastorage.h \
           ../../../../LIB/trunk/qt/jkqtpelements.h \
           ../../../../LIB/trunk/qt/jkqtmathtext.h \
           ../../../../LIB/trunk/qt/jkqtpbaseelements.h \
           ../../../../LIB/trunk/qt/jkqtplotter.h \
           ../../../../LIB/trunk/qt/jkqtptools.h \
           ../../../../LIB/trunk/qt/jkqttools.h \
           ../../../../LIB/trunk/qt/qtlogfile.h \
           ../../../../LIB/trunk/tools.h \
           ../../../../LIB/trunk/datatable2.h \
           optionsdialog.h

SOURCES += main.cpp \
           mainwindow.cpp \
           ../../../../LIB/trunk/qt/jkqtpbaseplotter.cpp \
           ../../../../LIB/trunk/qt/jkqtpdatastorage.cpp \
           ../../../../LIB/trunk/qt/jkqtpelements.cpp \
           ../../../../LIB/trunk/qt/jkqtmathtext.cpp \
           ../../../../LIB/trunk/qt/jkqtpbaseelements.cpp \
           ../../../../LIB/trunk/qt/jkqtplotter.cpp \
           ../../../../LIB/trunk/qt/jkqtptools.cpp \
           ../../../../LIB/trunk/qt/jkqttools.cpp \
           ../../../../LIB/trunk/qt/qtlogfile.cpp \
           ../../../../LIB/trunk/tools.cpp \
           ../../../../LIB/trunk/datatable2.cpp \
           optionsdialog.cpp

FORMS =    optionsdialog.ui

RESOURCES += quickfit3.qrc \
             ../../../../LIB/trunk/qt/jkqtpbaseplotter.qrc

TRANSLATIONS= ../output/translations/de.ts

INCLUDEPATH += ../../../../LIB/trunk/

QT += gui xml svg
CONFIG += console



MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
