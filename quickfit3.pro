TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += .
INCLUDEPATH += .

QMAKE_CXXFLAGS += -fexceptions -msse -msse2 -mfpmath=387,sse -O2
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
           ../lib/qt/jkqtpbaseplotter.h \
           ../lib/qt/jkqtpdatastorage.h \
           ../lib/qt/jkqtpelements.h \
           ../lib/qt/jkqtmathtext.h \
           ../lib/qt/jkqtpbaseelements.h \
           ../lib/qt/jkqtplotter.h \
           ../lib/qt/jkqtptools.h \
           ../lib/qt/jkqttools.h \
           ../lib/qt/qtlogfile.h \
           ../lib/tools.h \
           ../lib/datatable2.h \
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
           ../lib/qt/jkqtpbaseplotter.cpp \
           ../lib/qt/jkqtpdatastorage.cpp \
           ../lib/qt/jkqtpelements.cpp \
           ../lib/qt/jkqtmathtext.cpp \
           ../lib/qt/jkqtpbaseelements.cpp \
           ../lib/qt/jkqtplotter.cpp \
           ../lib/qt/jkqtptools.cpp \
           ../lib/qt/jkqttools.cpp \
           ../lib/qt/qtlogfile.cpp \
           ../lib/tools.cpp \
           ../lib/datatable2.cpp \
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
             ../lib/qt/jkqtpbaseplotter.qrc

TRANSLATIONS= translations/de.ts


QT += gui xml svg
CONFIG += console


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
DESTDIR = ./
