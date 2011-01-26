TEMPLATE = lib
CONFIG += plugin


TARGET = qfrdrfcs
DEPENDPATH += ./
DESTDIR = ../../output/plugins
DLLDESTDIR = $$DESTDIR
# ./
LIBS += -L../../output/ -lquickfit3lib -lquickfit3widgets

# Input
HEADERS += dlgcsvparameters.h \
           alv5000tools.h \
           qfrdrfcscorrelationeditor.h \
           qfrdrfcsdata.h \
           qfrdrfcsrateeditor.h \
		   qfprdrfcs.h \
		   qfrdrfcsdatainterface.h

SOURCES += dlgcsvparameters.cpp \
           alv5000tools.cpp \
           qfrdrfcscorrelationeditor.cpp \
           qfrdrfcsdata.cpp \
           qfrdrfcsrateeditor.cpp \
		   qfprdrfcs.cpp

FORMS = dlg_csvparameters.ui

RESOURCES += qfrdrfcs.qrc

TRANSLATIONS= ../../output/translations/de.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/

QT += gui xml svg

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/

