TEMPLATE = lib
TARGET = quickfit3lib
DEPENDPATH += .

INCLUDEPATH += ../../../../LIB/trunk/

QT += gui xml
DESTDIR = ./
DLLDESTDIR = ../output

# Input
HEADERS += dlgnewproperty.h \
           programoptions.h \
           qehtreeview.h \
           qfevaluationeditor.h \
           qfevaluationitem.h \
           qfevaluationitemfactory.h \
           qfevaluationpropertyeditor.h \
           qfpluginevaluation.h \
           qfpluginrawdata.h \
           qfpluginreportwrapper.h \
           qfproject.h \
           qfprojectrawdatamodel.h \
           qfprojecttreemodel.h \
           qfproperties.h \
           qfrawdataeditor.h \
           qfrawdatapropertyeditor.h \
           qfrawdatarecord.h \
           qfrawdatarecordfactory.h \
           qfrdrpropertymodel.h \
           qfrdrresultsmodel.h

SOURCES += dlgnewproperty.cpp \
           programoptions.cpp \
           qehtreeview.cpp \
           qfevaluationeditor.cpp \
           qfevaluationitem.cpp \
           qfevaluationitemfactory.cpp \
           qfevaluationpropertyeditor.cpp \
           qfproject.cpp \
           qfprojectrawdatamodel.cpp \
           qfprojecttreemodel.cpp \
           qfproperties.cpp \
           qfrawdataeditor.cpp \
           qfrawdatapropertyeditor.cpp \
           qfrawdatarecord.cpp \
           qfrawdatarecordfactory.cpp \
           qfrdrpropertymodel.cpp \
           qfrdrresultsmodel.cpp

FORMS +=   dlg_newproperty.ui

RESOURCES +=

TRANSLATIONS= ../output/translations/de.ts

INCLUDEPATH +=


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
