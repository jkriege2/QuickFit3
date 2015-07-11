#
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#	
#	 last modification: $LastChangedDate$  (revision $Rev$)
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

INCLUDEPATH += ../global_lib/ \
               . \
               ../lib/ \
               ../plugins/interfaces/

include(../libquickfit.pri)
include(../global_lib/qt/qfvisualize3dshaders.pri)

include(../quickfit3_configqtwidgets.pri)

#DEFINES += JKQTBP_AUTOTIMER JKQTBP_DEBUGTIMING


DESTDIR = $$QFOUTPUT

#DEFINES += JKQTBP_AUTOTIMER
# Input
HEADERS += ../global_lib/qt/jkqtpbaseplotter.h \
           ../global_lib/qt/jkqtpdatastorage.h \
           ../global_lib/qt/jkqtpelements.h \
           ../global_lib/qt/jkqtpgeoelements.h \
           ../global_lib/qt/jkqtmathtext.h \
           ../global_lib/qt/jkqtpbaseelements.h \
           ../global_lib/qt/jkqtplotter.h \
           ../global_lib/qt/jkqtptools.h \
           ../global_lib/qt/jkqttools.h \
           ../global_lib/qt/flowlayout.h \
           ../global_lib/qt/jkqtfastplotter.h \
           ../global_lib/qt/qtlogfile.h \
           ../global_lib/tools.h \
           ../global_lib/datatable2.h \
           ../global_lib/qt/jkverticalscrollarea.h \
           ../global_lib/qt/colorcombobox.h \
           ../global_lib/qt/jkstyledbutton.h \
           ../global_lib/qt/qenhancedlineedit.h \
           ../global_lib/qt/qspecialtoolbutton.h \
           ../global_lib/qt/qmodernprogresswidget.h \
           ../global_lib/qt/qmoretextobject.h \
           qehtreeview.h \
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
    ../global_lib/lib_imexport.h \
    ../global_lib/qt/jkqtpimagetools.h \
    ../global_lib/qt/jkqtpimageelements.h \
    filters.h \
    ../global_lib/qt/qenhancedcombobox.h \
    qfshuttercombobox.h \
    ../global_lib/qt/qrecentfilesmenu.h \
    qfplayercontrols.h \
    ../global_lib/qt/jkqtpoverlayelements.h \
    qfsimplefitparameterinterface.h \
    qfsimplefitparameterswidget.h \
    ../global_lib/image_tools.h \
    ../global_lib/qt/qkeysequenceedit.h \
    qffilterchangercombobox.h \
    ../plugins/interfaces/qfextensionlightsource.h \
    ../plugins/interfaces/qfextensionfilterchanger.h \
    qflightsourcecombobox.h \
    qfshutterconfigwidget.h \
    qffilterchangerconfigwidget.h \
    qflightsourceconfigwidget.h \
    qflightsourceconfigwidgetthread.h \
    ../global_lib/qt/qprogresslistwidget.h \
    ../global_lib/qt/qsettingsmodel.h \
    ../global_lib/qt/linestylecombobox.h \
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
    ../global_lib/qt/jkqtpparsedfunctionelements.h \
    ../global_lib/jkmathparser.h \
    qfhtmlhighlighter.h \
    qfplotter.h \
    qfplotterprivate.h \
    qfselectrdrdialog.h \
    qfrdrcolumngraphicscombobox.h \
    qfplottercopytotabledialog.h \
    qfmathparserxfunctionlinegraph.h \
    doubledatacutsliders.h \
    ../global_lib/qt/jkautooutputtimer.h \
    qfrecentfilesmenu.h \
    qfstageconfigwidgetthread.h \
    qfstageconfigwidget.h \
    ../global_lib/qt/fillstylecombobox.h \
    qfsimplecurvefitdialog.h \
    qfparametercorrelationview.h \
    qfmeasurementdevicecombobox.h \
    qfmeasurementdeviceconfigwidgetthread.h \
    qfmeasurementdeviceconfigwidget.h \
    qfrdrimagetorunpreview.h \
    qfrdrimagemaskedittools.h \
    qfrdrimagemaskbyintensity.h \
    ../global_lib/qt/qfluteditor.h \
    ../global_lib/highrestimer.h \
    ../global_lib/ticktock.h \
    ../global_lib/qt/qfvisualize3dglwidget.h \
    ../global_lib/qt/qfvisualize3dgleditor.h \
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
    qfrdrmaskbyoverviewimagedlg.h


SOURCES += ../global_lib/qt/jkqtpbaseplotter.cpp \
           ../global_lib/qt/jkqtpdatastorage.cpp \
           ../global_lib/qt/jkqtpelements.cpp \
           ../global_lib/qt/jkqtpgeoelements.cpp \
           ../global_lib/qt/jkqtmathtext.cpp \
           ../global_lib/qt/jkqtpbaseelements.cpp \
           ../global_lib/qt/jkqtplotter.cpp \
           ../global_lib/qt/jkqtptools.cpp \
           ../global_lib/qt/jkqttools.cpp \
            ../global_lib/qt/flowlayout.cpp \
           ../global_lib/qt/qtlogfile.cpp \
           ../global_lib/qt/jkqtfastplotter.cpp \
           ../global_lib/tools.cpp \
           ../global_lib/datatable2.cpp \
           ../global_lib/qt/jkverticalscrollarea.cpp \
           ../global_lib/qt/colorcombobox.cpp \
           ../global_lib/qt/jkstyledbutton.cpp \
           ../global_lib/qt/qenhancedlineedit.cpp \
           ../global_lib/qt/qspecialtoolbutton.cpp \
           ../global_lib/qt/qmodernprogresswidget.cpp \
           ../global_lib/qt/qmoretextobject.cpp \
           qehtreeview.cpp \
           datacutslider.cpp \
           qffitparameterwidget.cpp \
    qfstagecombobox.cpp \
    qfcameraconfigcombobox.cpp \
    qfcameracombobox.cpp \
    objectives.cpp \
    ../global_lib/qt/jkqtpimagetools.cpp \
    ../global_lib/qt/jkqtpimageelements.cpp \
    filters.cpp \
    ../global_lib/qt/qenhancedcombobox.cpp \
    qfshuttercombobox.cpp \
    ../global_lib/qt/qrecentfilesmenu.cpp \
    qfplayercontrols.cpp \
    ../global_lib/qt/jkqtpoverlayelements.cpp \
    qfsimplefitparameterswidget.cpp \
    ../global_lib/image_tools.cpp \
    ../global_lib/qt/qkeysequenceedit.cpp \
    qffilterchangercombobox.cpp \
    qflightsourcecombobox.cpp \
    qfshutterconfigwidget.cpp \
    qffilterchangerconfigwidget.cpp \
    qflightsourceconfigwidget.cpp \
    qflightsourceconfigwidgetthread.cpp \
    ../global_lib/qt/qprogresslistwidget.cpp \
    ../global_lib/qt/qsettingsmodel.cpp \
    ../global_lib/qt/linestylecombobox.cpp \
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
    ../global_lib/qt/jkqtpparsedfunctionelements.cpp \
    ../global_lib/jkmathparser.cpp \
    qfhtmlhighlighter.cpp \
    qfplotter.cpp \
    qfplotterprivate.cpp \
    qfselectrdrdialog.cpp \
    qfrdrcolumngraphicscombobox.cpp \
    qfplottercopytotabledialog.cpp \
    qfmathparserxfunctionlinegraph.cpp \
    doubledatacutsliders.cpp \
    ../global_lib/qt/jkautooutputtimer.cpp \
    qfrecentfilesmenu.cpp \
    qfstageconfigwidgetthread.cpp \
    qfstageconfigwidget.cpp \
    ../global_lib/qt/fillstylecombobox.cpp \
    qfsimplecurvefitdialog.cpp \
    qfparametercorrelationview.cpp \
    qfmeasurementdevicecombobox.cpp \
    qfmeasurementdeviceconfigwidgetthread.cpp \
    qfmeasurementdeviceconfigwidget.cpp \
    qfrdrimagetorunpreview.cpp \
    qfrdrimagemaskedittools.cpp \
    qfrdrimagemaskbyintensity.cpp \
    ../global_lib/qt/qfluteditor.cpp \
    ../global_lib/highrestimer.cpp \
    ../global_lib/qt/qfvisualize3dglwidget.cpp \
    ../global_lib/qt/qfvisualize3dgleditor.cpp \
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
    qfrdrmaskbyoverviewimagedlg.cpp


FORMS += qf3objectiveeditor.ui \
    qf3filtereditor.ui \
    qfselectionlistdialog.ui \
    qfselectrdrdialog.ui \
    qfplottercopytotabledialog.ui \
    qfsimplecurvefitdialog.ui \
    qfrdrimagemaskbyintensity.ui \
    ../global_lib/qt/qfvisualize3dgleditor.ui \
    qffitfunctionselectdialog.ui \
    qfoverlayplotdialog.ui \
    qfselectfileslistwidget.ui \
    qfsimpleplotview.ui \
    qfsimpleplotwidget.ui \
    qfmathparserwidget.ui \
    qfmathparserwidgetsmall.ui \
    qfimageplot.ui \
    qfrdrmaskbyoverviewimagedlg.ui


RESOURCES += ../global_lib/qt/jkqtpbaseplotter.qrc \
             ../global_lib/qt/qspecialtoolbutton.qrc \
             ../global_lib/qt/jkstyledbutton.qrc \
             ../global_lib/qt/libqt.qrc \
             ../global_lib/qt/qkeysequenceedit.qrc \
             ../global_lib/qt/numberedit.qrc \
             libqf3widgets.qrc \
    ../global_lib/qt/qprogresslistwidget.qrc \
    ../global_lib/qt/qtlogfile.qrc \
    ../extlibs/xits.qrc \
    ../global_lib/qt/qfvisualize3d.qrc


TRANSLATIONS= ./translations/de.quickfit3widgets.ts

INCLUDEPATH += ../global_lib/ \
               ../global_lib/qt/

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
