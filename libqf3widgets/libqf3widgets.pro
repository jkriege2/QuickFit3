#
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
TARGET = quickfit3widgets

include(../quickfit3.pri)

release {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$QFOUTPUT")
} else {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$QFOUTPUT")
}

DEPENDPATH += .

INCLUDEPATH += . \
               ../plugins/interfaces/ \
               ../extlibsb040/StatisticsTools/

include(../libquickfit.pri)
include(../extlibsb040/jkqtplotters.pri)
include(qfvisualize3dshaders.pri)

include(../quickfit3_configqtwidgets.pri)

#DEFINES += JKQTBP_AUTOTIMER JKQTBP_DEBUGTIMING


DESTDIR = $$QFOUTPUT

#DEFINES += JKQTBP_AUTOTIMER
# Input
HEADERS += jkqttools.h \
           flowlayout.h \
           qtlogfile.h \
           jkverticalscrollarea.h \
           colorcombobox.h \
           jkstyledbutton.h \
           qspecialtoolbutton.h \
           qmodernprogresswidget.h \
           qmoretextobject.h \
           datacutslider.h \
           qffitparameterwidget.h \
    qfstagecombobox.h \
    qfcameraconfigcombobox.h \
    qfcameracombobox.h \
    objectives.h \
    ../plugins/interfaces/qfrdrfcsdatainterface.h \
    ../plugins/interfaces/qfrdrcountratesinterface.h \
    ../plugins/interfaces/qfextensionlinearstage.h \
    ../plugins/interfaces/qfextensioncamera.h \
    libwid_imexport.h \
    filters.h \
    qfshuttercombobox.h \
    qrecentfilesmenu.h \
    qfplayercontrols.h \
    qfsimplefitparameterinterface.h \
    qfsimplefitparameterswidget.h \
    qkeysequenceedit.h \
    qffilterchangercombobox.h \
    ../plugins/interfaces/qfextensionlightsource.h \
    ../plugins/interfaces/qfextensionfilterchanger.h \
    qflightsourcecombobox.h \
    qfshutterconfigwidget.h \
    qffilterchangerconfigwidget.h \
    qflightsourceconfigwidget.h \
    qflightsourceconfigwidgetthread.h \
    qprogresslistwidget.h \
    qsettingsmodel.h \
    linestylecombobox.h \
    qfacquisitionconfigcombobox.h \
    qfacquisitioncombobox.h \
    qfshutterconfigwidgetthread.h \
    qffilterchangerconfigwidgetthread.h \
    qffitfunctioncombobox.h \
    qffitalgorithmcombobox.h \
    qffitfunctionplottools.h \
    qfhistogramview.h \
    qfselectionlistdialog.h \
    qfcompletertextedit.h \
    qfqtscripthighlighter.h \
    qfhtmlhighlighter.h \
    qfplotter.h \
    qfplotterprivate.h \
    qfselectrdrdialog.h \
    qfrdrcolumngraphicscombobox.h \
    qfplottercopytotabledialog.h \
    qfmathparserxfunctionlinegraph.h \
    doubledatacutsliders.h \
    jkautooutputtimer.h \
    qfrecentfilesmenu.h \
    qfstageconfigwidgetthread.h \
    qfstageconfigwidget.h \
    fillstylecombobox.h \
    qfsimplecurvefitdialog.h \
    qfparametercorrelationview.h \
    qfmeasurementdevicecombobox.h \
    qfmeasurementdeviceconfigwidgetthread.h \
    qfmeasurementdeviceconfigwidget.h \
    qfrdrimagetorunpreview.h \
    qfrdrimagemaskedittools.h \
    qfrdrimagemaskbyintensity.h \
    qfluteditor.h \
    qfvisualize3dglwidget.h \
    qfvisualize3dgleditor.h \
    qffitfunctionselectdialog.h \
    qfoverlayplotdialog.h \
    ../plugins/interfaces/qfgetplotdatainterface.h \
    qfselectfileslistwidget.h \
    qfsimpleplotview.h \
    qfsimpleplotwidget.h\
    qffunctionreferencetool.h \
    qfmathparserwidget.h \
    qfmathparserwidgetsmall.h \
    qfimageplot.h \
    qftableview.h \
    qfrdrmaskbyoverviewimagedlg.h\
    numberedit.h \
    qfasttablelabel.h \
    qfpixelsizeedit.h \
    qfframerangeedit.h \
    qfcroppixelsedit.h \
    qcheckablestringlistmodel.h


SOURCES +=  \
           jkqttools.cpp \
            flowlayout.cpp \
           qtlogfile.cpp \
           jkverticalscrollarea.cpp \
           colorcombobox.cpp \
           jkstyledbutton.cpp \
           qspecialtoolbutton.cpp \
           qmodernprogresswidget.cpp \
           qmoretextobject.cpp \
           datacutslider.cpp \
           qffitparameterwidget.cpp \
    qfstagecombobox.cpp \
    qfcameraconfigcombobox.cpp \
    qfcameracombobox.cpp \
    objectives.cpp \
    filters.cpp \
    qfshuttercombobox.cpp \
    qrecentfilesmenu.cpp \
    qfplayercontrols.cpp \
    qfsimplefitparameterswidget.cpp \
    qkeysequenceedit.cpp \
    qffilterchangercombobox.cpp \
    qflightsourcecombobox.cpp \
    qfshutterconfigwidget.cpp \
    qffilterchangerconfigwidget.cpp \
    qflightsourceconfigwidget.cpp \
    qflightsourceconfigwidgetthread.cpp \
    qprogresslistwidget.cpp \
    qsettingsmodel.cpp \
    linestylecombobox.cpp \
    qfacquisitionconfigcombobox.cpp \
    qfacquisitioncombobox.cpp \
    qfshutterconfigwidgetthread.cpp \
    qffilterchangerconfigwidgetthread.cpp \
    qffitfunctioncombobox.cpp \
    qffitalgorithmcombobox.cpp \
    qffitfunctionplottools.cpp \
    qfhistogramview.cpp \
    qfselectionlistdialog.cpp \
    qfqtscripthighlighter.cpp \
    qfcompletertextedit.cpp \
    qfhtmlhighlighter.cpp \
    qfplotter.cpp \
    qfplotterprivate.cpp \
    qfselectrdrdialog.cpp \
    qfrdrcolumngraphicscombobox.cpp \
    qfplottercopytotabledialog.cpp \
    qfmathparserxfunctionlinegraph.cpp \
    doubledatacutsliders.cpp \
    jkautooutputtimer.cpp \
    qfrecentfilesmenu.cpp \
    qfstageconfigwidgetthread.cpp \
    qfstageconfigwidget.cpp \
    fillstylecombobox.cpp \
    qfsimplecurvefitdialog.cpp \
    qfparametercorrelationview.cpp \
    qfmeasurementdevicecombobox.cpp \
    qfmeasurementdeviceconfigwidgetthread.cpp \
    qfmeasurementdeviceconfigwidget.cpp \
    qfrdrimagetorunpreview.cpp \
    qfrdrimagemaskedittools.cpp \
    qfrdrimagemaskbyintensity.cpp \
    qfluteditor.cpp \
    qfvisualize3dglwidget.cpp \
    qfvisualize3dgleditor.cpp \
    qffitfunctionselectdialog.cpp \
    qfoverlayplotdialog.cpp \
    qfselectfileslistwidget.cpp \
    qfsimpleplotview.cpp \
    qfsimpleplotwidget.cpp \
    qffunctionreferencetool.cpp \
    qfmathparserwidget.cpp \
    qfmathparserwidgetsmall.cpp \
    qfimageplot.cpp \
    qftableview.cpp \
    qfrdrmaskbyoverviewimagedlg.cpp \
    numberedit.cpp \
    qfasttablelabel.cpp \
    qfpixelsizeedit.cpp \
    qfframerangeedit.cpp \
    qfcroppixelsedit.cpp \
    qcheckablestringlistmodel.cpp


FORMS += qf3objectiveeditor.ui \
    qf3filtereditor.ui \
    qfselectionlistdialog.ui \
    qfselectrdrdialog.ui \
    qfplottercopytotabledialog.ui \
    qfsimplecurvefitdialog.ui \
    qfrdrimagemaskbyintensity.ui \
    qfvisualize3dgleditor.ui \
    qffitfunctionselectdialog.ui \
    qfoverlayplotdialog.ui \
    qfselectfileslistwidget.ui \
    qfsimpleplotview.ui \
    qfsimpleplotwidget.ui \
    qfmathparserwidget.ui \
    qfmathparserwidgetsmall.ui \
    qfimageplot.ui \
    qfrdrmaskbyoverviewimagedlg.ui


RESOURCES += qspecialtoolbutton.qrc \
             jkstyledbutton.qrc \
             qkeysequenceedit.qrc \
             numberedit.qrc \
             libqf3widgets.qrc \
    qprogresslistwidget.qrc \
    qtlogfile.qrc \
    ../extlibs/xits.qrc \
    qfvisualize3d.qrc


TRANSLATIONS= ./translations/de.quickfit3widgets.ts

CONFIG += dll lib_bundle exceptions rtti stl create_prl

DEFINES += QFWIDLIB_LIBRARY LIB_IN_DLL LIB_EXPORT_LIBRARY USE_XITS_FONTS
#AUTOLOAD_XITS_FONTS


ASSETSScript_FILES.files = ./assets/highlighter/qtscript/*.*
ASSETSScript_FILES.path = $${QFOUTPUT}/assets/highlighter/qtscript
ASSETSSHTML_FILES.files = ./assets/highlighter/html/*.*
ASSETSSHTML_FILES.path = $${QFOUTPUT}/assets/highlighter/html
ASSETSShigh_FILES.files = ./assets/highlighter/*.*
ASSETSShigh_FILES.path = $${QFOUTPUT}/assets/highlighter

INSTALLS += ASSETSScript_FILES ASSETSSHTML_FILES ASSETSShigh_FILES



XITS_SRC_DISTRIBUTED.files = ../extlibs/xits/*.zip
XITS_SRC_DISTRIBUTED.path = $${QFOUTPUT}/source/extlibs/xits/
XITS_SRC_DISTRIBUTED1.files = ../extlibs/xits/*.txt
XITS_SRC_DISTRIBUTED1.path = $${QFOUTPUT}/source/extlibs/xits/


INSTALLS += XITS_SRC_DISTRIBUTED XITS_SRC_DISTRIBUTED1
