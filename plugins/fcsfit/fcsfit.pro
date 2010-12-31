TEMPLATE = lib
CONFIG += plugin


TARGET = qfevalfcsfit
DEPENDPATH += ./
DLLDESTDIR = ../../output/plugins
DESTDIR = ./
LIBS += -L../../lib/ -lquickfit3lib -L../fcs/ -lqfrdrfcs

# Input
HEADERS += qfpevalfcsfit.h \
           qffcsfitevaluation.h \
           qffcsfitevaluationeditor.h \
           ../../../../../LIB/trunk/tools.h \
           ../../../../../LIB/trunk/datatable2.h \
           ../../../../../LIB/trunk/qt/jkqtpbaseplotter.h \
           ../../../../../LIB/trunk/qt/jkqtpdatastorage.h \
           ../../../../../LIB/trunk/qt/jkqtpelements.h \
           ../../../../../LIB/trunk/qt/jkqtmathtext.h \
           ../../../../../LIB/trunk/qt/jkqtpbaseelements.h \
           ../../../../../LIB/trunk/qt/jkqtplotter.h \
           ../../../../../LIB/trunk/qt/jkqtptools.h \
           ../../../../../LIB/trunk/qt/jkqttools.h

SOURCES += qfpevalfcsfit.cpp \
           qffcsfitevaluation.cpp \
           qffcsfitevaluationeditor.cpp \
           ../../../../../LIB/trunk/tools.cpp \
           ../../../../../LIB/trunk/datatable2.cpp \
           ../../../../../LIB/trunk/qt/jkqtpbaseplotter.cpp \
           ../../../../../LIB/trunk/qt/jkqtpdatastorage.cpp \
           ../../../../../LIB/trunk/qt/jkqtpelements.cpp \
           ../../../../../LIB/trunk/qt/jkqtmathtext.cpp \
           ../../../../../LIB/trunk/qt/jkqtpbaseelements.cpp \
           ../../../../../LIB/trunk/qt/jkqtplotter.cpp \
           ../../../../../LIB/trunk/qt/jkqtptools.cpp \
           ../../../../../LIB/trunk/qt/jkqttools.cpp

FORMS =

RESOURCES += qfevalfcsfit.qrc

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/ \
               ../../../../../LIB/trunk/

QT += gui xml svg

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

