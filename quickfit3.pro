TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += .
INCLUDEPATH += .

#QMAKE_CXXFLAGS += -fexceptions -msse -msse2 -mfpmath=387,sse -O2
LIBS += -lgsl -lgslcblas
# Input
HEADERS += mainwindow.h \
           version.h \
           qfrawdatarecord.h \
           qfproject.h \
           qfrawdatapropertyeditor.h \
           qfrawdataeditor.h \
           qfrdrtable.h \
           qftablemodel.h \
           qfrdrtableeditor.h \
           ../../../LIB/trunk/qt/jkqtpbaseplotter.h \
           ../../../LIB/trunk/qt/jkqtpdatastorage.h \
           ../../../LIB/trunk/qt/jkqtpelements.h \
           ../../../LIB/trunk/qt/jkqtmathtext.h \
           ../../../LIB/trunk/qt/jkqtpbaseelements.h \
           ../../../LIB/trunk/qt/jkqtplotter.h \
           ../../../LIB/trunk/qt/jkqtptools.h \
           ../../../LIB/trunk/qt/jkqttools.h \
           ../../../LIB/trunk/qt/qtlogfile.h \
           ../../../LIB/trunk/tools.h \
           ../../../LIB/trunk/datatable2.h \
           dlgCSVParameters.h \
           dlgnewproperty.h \
           qfevaluationitem.h \
           qfevaluationeditor.h \
           qfevaluationpropertyeditor.h \
           optionsdialog.h \
           programoptions.h \
           qfprojectrawdatamodel.h \
           qehtreeview.h \
           qfrdrfcsdata.h \
           qfrdrfcscorrelationeditor.h \
           qfrdrfcsrateeditor.h \
           qfrdrresultsmodel.h \
           qfprojecttreemodel.h \
           qfrawdatarecordfactory.h \
           qfevaluationitemfactory.h \
           numberedit.h \
           qftoolwidgets.h \
           qfrdrpropertymodel.h \
           alv5000tools.h \
           qffcsfitevaluation.h \
           qffcsfitevaluationeditor.h

SOURCES += main.cpp \
           mainwindow.cpp \
           qfrawdatarecord.cpp \
           qfproject.cpp \
           qfrawdatapropertyeditor.cpp \
           qfrawdataeditor.cpp \
           qfrdrtable.cpp \
           qftablemodel.cpp \
           qfrdrtableeditor.cpp \
           ../../../LIB/trunk/qt/jkqtpbaseplotter.cpp \
           ../../../LIB/trunk/qt/jkqtpdatastorage.cpp \
           ../../../LIB/trunk/qt/jkqtpelements.cpp \
           ../../../LIB/trunk/qt/jkqtmathtext.cpp \
           ../../../LIB/trunk/qt/jkqtpbaseelements.cpp \
           ../../../LIB/trunk/qt/jkqtplotter.cpp \
           ../../../LIB/trunk/qt/jkqtptools.cpp \
           ../../../LIB/trunk/qt/jkqttools.cpp \
           ../../../LIB/trunk/qt/qtlogfile.cpp \
           ../../../LIB/trunk/tools.cpp \
           ../../../LIB/trunk/datatable2.cpp \
           dlgCSVParameters.cpp \
           dlgnewproperty.cpp \
           qfevaluationitem.cpp \
           qfevaluationeditor.cpp \
           qfevaluationpropertyeditor.cpp \
           optionsdialog.cpp \
           programoptions.cpp \
           qfprojectrawdatamodel.cpp \
           qehtreeview.cpp \
           qfrdrfcsdata.cpp \
           qfrdrfcscorrelationeditor.cpp \
           qfrdrfcsrateeditor.cpp \
           qfrdrresultsmodel.cpp \
           qfprojecttreemodel.cpp \
           qfrawdatarecordfactory.cpp \
           qfevaluationitemfactory.cpp \
           numberedit.cpp \
           qftoolwidgets.cpp \
           qfrdrpropertymodel.cpp \
           alv5000tools.cpp \
           qffcsfitevaluation.cpp \
           qffcsfitevaluationeditor.cpp

FORMS = dlg_csvparameters.ui \
        dlg_newproperty.ui \
        optionsdialog.ui

RESOURCES += quickfit3.qrc \
             ../../../LIB/trunk/qt/jkqtpbaseplotter.qrc

TRANSLATIONS= translations/de.ts

INCLUDEPATH += ../../../LIB/trunk/

QT += gui xml svg
CONFIG += console


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
DESTDIR = ./
