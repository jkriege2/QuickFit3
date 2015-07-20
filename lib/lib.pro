
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#
#	
#
#    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).
#
#    This software is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License (LGPL) as published by
#    the Free Software Foundation, either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License (LGPL) for more details.
#
#    You should have received a copy of the GNU Lesser General Public License (LGPL)
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

TEMPLATE = lib
TARGET = quickfit3lib
DEPENDPATH += .

include(../extlibs/zlib.pri)
include(../extlibsb040/tinymat.pri)
include(../quickfit3.pri)
include(../quickfit3_configqtwidgets.pri)
include(../extlibs/quazip.pri)
include(../extlibs/faddeeva.pri)



release {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$QFOUTPUT")
} else {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$QFOUTPUT")
}


INCLUDEPATH += .
INCLUDEPATH += ../extlibsb040/StatisticsTools/
INCLUDEPATH += ../extlibsb040/TinyMAT/




DESTDIR = $$QFOUTPUT

# Input
HEADERS += dlgnewproperty.h \
           qehtreeview.h \
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
           qfrdrcolumngraphsinterface.h \
    ../extlibsb040/StatisticsTools/statistics_tools.h \
    qtriple.h \
    qvisiblehandlesplitter.h \
    lib_imexport.h \
    qfhtmldelegate.h \
    qfprojecttreemodelnode.h \
    qfstyleditemdelegate.h \
    binarydatatools.h \
    qfmathtools.h \
    qfpluginimporters.h \
    qfimportermanager.h \
    qfimporter.h \
    qfimporterimageseries.h \
    qftcspcreader.h \
    qfenhancedlineedit.h \
    qfstyledbutton.h \
    qfcompleterfromfile.h \
    qfpluginoptionsdialog.h \
    dlgqfprogressdialog.h \
    qfxmlstreamwriter.h \
    qfpropertyitemdelegate.h \
    qfdoubleedit.h \
    dlg_qffileeditdialog.h \
    qfversion.h \
    qfhistogramservice.h \
    matlabtools.h \
    qftextdocumentprintpreview.h \
    qfmatchrdrfunctor.h \
    qfpreprocessormagic.h \
    qfpreprocessormagic_private.h \
    qfautooutputtimer.h \
    qfrdrcombobox.h \
    qfresultsgroupcombobox.h \
    qfrawdataresultscombobox.h \
    qfelidedlabel.h \
    qfmathparser.h \
    ../extlibs/MersenneTwister.h \
    qfrawdatapropertyeditor_private.h \
    qfglobalfittool.h \
    qfmanyfilessettings.h \
    qfhtmlhelptools.h \
    qffitfunctionvalueinputtable.h \
    qffitfunctionvalueinputdelegate.h \
    qffitparameterbasicinterface.h \
    qftreeviewcombobox.h \
    qfsimpletreemodel.h \
    qfdoubleslider.h \
    qffitfunctionparsed.h \
    qfparametercorrelationservice.h \
    datatools.h \
    qfmathparsertools.h \
    qfmathparserdefaultlib.h \
    qfevaluationpropertyeditorprivate.h \
    imagetools.h \
    qfenhancedtabwidget.h \
    qfenhancedcombobox.h \
    qffitalgorithmparameterstorage.h \
    qftabledelegate.h \
    qfcfitfunction.h \
    qfdlgcsvparameters.h \
    sdk_fitfunctions/quickfit-model-tools.h \
    sdk_fitfunctions/quickfit-model-header.h \
    qflibraryfitfunction.h \
    qfwizard.h \
    qfhelptoolbutton.h \
    qfrdrrunselection.h \
    qfrdrimagetoruninterface.h \
    qffitalgorithmerrorestimatemodewidget.h \
    qfsimpleplotservice.h \
    ../version.h \
    qfcollapsibleframe.h \
    qfcollapsiblescrollarea.h \
    qfrotatablelabel.h \
    qfcompleterproperties.h \
    qfimagemetadatatool.h \
    qflistprogressdialog.h \
    qfenhancedlabel.h \
    qfhelpaction.h \
    qfrdrtableinterface.h \
    qftableservice.h \
    qfrdrimagemask.h \
    qfrdrimageselectioninterface.h \
    qfthreadingtools.h \
    qfenhancedplaintextedit.h \
    qffitfunctionbase.h \
    qconfigcomboboxwidget.h \
    qftablemodel.h \
    qfevalpropertycopyparsertabledlg.h \
    qffitfunctionnd.h \
    qfexporter.h \
    qfexporterimageseries.h \
    qfexportermanager.h \
    qfpluginexporters.h \
    qfexporttotabledialog.h \
    qfrdrtablecombobox.h \
    qfpseudotreemodel.h \
    qfrdrannotationinterface.h \
    qfrdrannotationtools.h \
    qfserialconnection.h \
    qf3comportmanager.h \
    datatable2.h \
    image_tools.h \
    jkmathparser.h \
    highrestimer.h \
    ticktock.h \
    jkiniparser2.h \
    cpptools.h


SOURCES += dlgnewproperty.cpp \
           qehtreeview.cpp \
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
    ../extlibsb040/StatisticsTools/statistics_tools.cpp \
    qvisiblehandlesplitter.cpp \
    qfpluginrawdata.cpp \
    qfhtmldelegate.cpp \
    qffitfunction.cpp \
    qfprojecttreemodelnode.cpp \
    qfstyleditemdelegate.cpp \
    binarydatatools.cpp \
    qfmathtools.cpp \
    qfimportermanager.cpp \
    qfimporterimageseries.cpp \
    qftcspcreader.cpp \
    qfpluginservices.cpp \
    qfenhancedlineedit.cpp \
    qfstyledbutton.cpp \
    qfcompleterfromfile.cpp \
    qfpluginoptionsdialog.cpp \
    dlgqfprogressdialog.cpp \
    qfxmlstreamwriter.cpp \
    qfpropertyitemdelegate.cpp \
    qfdoubleedit.cpp \
    dlg_qffileeditdialog.cpp \
    qfhistogramservice.cpp \
    matlabtools.cpp \
    qftextdocumentprintpreview.cpp \
    qfmatchrdrfunctor.cpp \
    qfautooutputtimer.cpp \
    qfrdrcombobox.cpp \
    qfresultsgroupcombobox.cpp \
    qfrawdataresultscombobox.cpp \
    qfelidedlabel.cpp \
    qfversion.cpp \
    qfmathparser.cpp \
    qfrawdatapropertyeditor_private.cpp \
    qfglobalfittool.cpp \
    qfmanyfilessettings.cpp \
    qfhtmlhelptools.cpp \
    qffitfunctionvalueinputtable.cpp \
    qffitfunctionvalueinputdelegate.cpp \
    qffitparameterbasicinterface.cpp \
    qftreeviewcombobox.cpp \
    qfsimpletreemodel.cpp \
    qfdoubleslider.cpp \
    qffitfunctionparsed.cpp \
    qfparametercorrelationservice.cpp \
    datatools.cpp \
    qfmathparsertools.cpp \
    qfmathparserdefaultlib.cpp \
    qfevaluationpropertyeditorprivate.cpp \
    imagetools.cpp \
    qfenhancedtabwidget.cpp \
    qfenhancedcombobox.cpp \
    qffitalgorithmparameterstorage.cpp \
    qftabledelegate.cpp \
    qfdlgcsvparameters.cpp \
    qflibraryfitfunction.cpp \
    qfwizard.cpp \
    qfhelptoolbutton.cpp \
    qffitalgorithmerrorestimatemodewidget.cpp \
    qfsimpleplotservice.cpp \
    qfcollapsibleframe.cpp \
    qfcollapsiblescrollarea.cpp \
    qfrotatablelabel.cpp \
    qfcompleterproperties.cpp \
    qfimagemetadatatool.cpp \
    qflistprogressdialog.cpp \
    qfenhancedlabel.cpp \
    qfhelpaction.cpp \
    qftableservice.cpp \
    qfthreadingtools.cpp \
    qfenhancedplaintextedit.cpp \
    qffitfunctionbase.cpp \
    qconfigcomboboxwidget.cpp \
    qftablemodel.cpp \
    qfevalpropertycopyparsertabledlg.cpp \
    qffitfunctionnd.cpp \
    qfexporterimageseries.cpp \
    qfexportermanager.cpp \
    qfexporttotabledialog.cpp \
    qfrdrtablecombobox.cpp \
    qfpseudotreemodel.cpp \
    qfrdrannotationtools.cpp \
    qfserialconnection.cpp \
    qf3comportmanager.cpp\
    datatable2.cpp \
    image_tools.cpp \
    jkmathparser.cpp \
    highrestimer.cpp \
    jkiniparser2.cpp \
    cpptools.cpp

FORMS +=   dlg_newproperty.ui \
           dlg_fitalgorithmprogress.ui \
    dlg_qfprogress.ui \
    dlg_qffileeditdialog.ui \
    qfdlg_csvparameters.ui \
    qffitalgorithmerrorestimatemodewidget.ui \
    qflistprogressdialog.ui \
    qfevalpropertycopyparsertabledlg.ui \
    qfexporttotabledialog.ui

RESOURCES += \
    lib.qrc

TRANSLATIONS= ./translations/de.lib.ts


CONFIG += dll lib_bundle exceptions rtti stl create_prl

DEFINES += QFLIB_LIBRARY LIB_IN_DLL LIB_EXPORT_LIBRARY




FITFUNCTIONSSDK.files = ./sdk_fitfunctions/*
FITFUNCTIONSSDK.path = $${QFOUTPUT}/sdk/sdk_fitfunctions/

INSTALLS += FITFUNCTIONSSDK


SRC_DISTRIBUTED.files = ../extlibs/MersenneTwister.h

OTHER_FILES += \
    sdk_fitfunctions/readme.txt \
    sdk_fitfunctions/Makefile









