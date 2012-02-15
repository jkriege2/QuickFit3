TEMPLATE = lib
TARGET = quickfit3lib
DEPENDPATH += .

include(../quickfit3.pri)


release {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$QFOUTPUT")
} else {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$QFOUTPUT")
}


INCLUDEPATH += .


QT += gui xml

DESTDIR = $$QFOUTPUT

# Input
HEADERS += dlgnewproperty.h \
           dlgqffitalgorithmprogressdialog.h \
           qenhancedtableview.h \
           programoptions.h \
           qfevaluationeditor.h \
           qfevaluationitem.h \
           qfevaluationitemfactory.h \
           qfevaluationpropertyeditor.h \
           qfpluginevaluation.h \
           qfpluginrawdata.h \
           qfpluginservices.h \
           qfproject.h \
           qfprojectrawdatamodel.h \
           qfprojecttreemodel.h \
           qfproperties.h \
           qfrawdataeditor.h \
           qfrawdatapropertyeditor.h \
           qfrawdatarecord.h \
           qfrawdatarecordfactory.h \
           qfrdrpropertymodel.h \
           qfrdrresultsmodel.h \
           qffitfunction.h \
           qffitalgorithm.h \
           qffitfunctionmanager.h \
           qffitalgorithmmanager.h \
           qfpluginfitfunction.h \
           qfpluginfitalgorithm.h \
           qftools.h \
           qffitalgorithmthreaddedfit.h \
           qfextension.h \
           qfextensionmanager.h \
           qfevaluationresultsmodel.h \
           qfplugin.h \
           qflib_version.h \
           csvtools.h \
    qtriple.h \
    qvisiblehandlesplitter.h \
    ../version.h \
    lib_imexport.h \
    qfhtmldelegate.h \
    qfprojecttreemodelnode.h \
    qfstyleditemdelegate.h

SOURCES += dlgnewproperty.cpp \
           dlgqffitalgorithmprogressdialog.cpp \
           qenhancedtableview.cpp \
           programoptions.cpp \
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
           qfrdrresultsmodel.cpp \
           qffitalgorithm.cpp \
           qffitfunctionmanager.cpp \
           qffitalgorithmmanager.cpp \
           qftools.cpp \
           qffitalgorithmthreaddedfit.cpp \
           qfextensionmanager.cpp \
           qfevaluationresultsmodel.cpp \
           csvtools.cpp \
    qvisiblehandlesplitter.cpp \
    qfpluginrawdata.cpp \
    qfhtmldelegate.cpp \
    qffitfunction.cpp \
    qfprojecttreemodelnode.cpp \
    qfstyleditemdelegate.cpp

FORMS +=   dlg_newproperty.ui \
           dlg_fitalgorithmprogress.ui

RESOURCES += lib.qrc

TRANSLATIONS= ./translations/de.lib.ts

INCLUDEPATH +=

CONFIG += dll lib_bundle exceptions rtti stl create_prl

DEFINES += QFLIB_LIBRARY











