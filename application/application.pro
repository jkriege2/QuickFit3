#
#    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#	
#	 last modification: $LastChangedDate$  (revision $Rev$)
#
#    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).
#
#    This software is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

TEMPLATE = app
TARGET = quickfit3
DEPENDPATH += . \
               ../lib/ \
               ../libqf3widgets/
INCLUDEPATH += . \
               ../lib/ \
               ../libqf3widgets/ \
               ../../../../LIB/trunk/ \
               ../../../../LIB/trunk/qt

include(../quickfit3.pri)
include(../libquickfitwidgets.pri)


release {
    message("building $$TARGET $$TEMPLATE in RELEASE mode, output is in $$QFOUTPUT ")
} else {
    message("building $$TARGET $$TEMPLATE in DEBUG mode, output is in $$QFOUTPUT ")
}
macx {
    message("build system is macx")
}
win32 {
    message("build system is win32")
}
unix {
    message("build system is unix")
}


DESTDIR = $$QFOUTPUT

QMAKE_RPATHDIR += $$DESTDIR
# Input
HEADERS += mainwindow.h \
           optionsdialog.h \
    ../version.h \
    qfrdrreplacedialog.h \
    qfhtmlhelpwindow.h \
    dlgcontactauthors.h \
    dlgnewversion.h \
    dlgrdrsetproperty.h \
    dlgselectprojectsubset.h \
    dlgfixfilepaths.h \
    renamegroupsdialog.h \
    dlgsetrdrpropertybyregexp.h \
    userfitfunctionseditor.h \
    userfitfunctionmodel.h \
    userfitfunctiondelegate.h \
    dlgsetrdrpropertybyexpression.h \
    dlgsetrdrpropertybyexpressioneditor.h \
    dlgwelcomescreen.h \
    qfgroupandrolemodel.h \
    dlgeditgroupandrole.h \
    qfsplashscreen.h

SOURCES += main.cpp \
           mainwindow.cpp \
           optionsdialog.cpp \
    qfrdrreplacedialog.cpp \
    qfhtmlhelpwindow.cpp \
    dlgcontactauthors.cpp \
    dlgnewversion.cpp \
    dlgrdrsetproperty.cpp \
    dlgselectprojectsubset.cpp \
    dlgfixfilepaths.cpp \
    renamegroupsdialog.cpp \
    dlgsetrdrpropertybyregexp.cpp \
    userfitfunctionseditor.cpp \
    userfitfunctionmodel.cpp \
    userfitfunctiondelegate.cpp \
    dlgsetrdrpropertybyexpression.cpp \
    dlgsetrdrpropertybyexpressioneditor.cpp \
    dlgwelcomescreen.cpp \
    qfgroupandrolemodel.cpp \
    dlgeditgroupandrole.cpp \
    qfsplashscreen.cpp

FORMS =    optionsdialog.ui \
           about.ui \
           aboutplugins.ui \
    qfrdrreplacedialog.ui \
    dlgcontactauthors.ui \
    dlgnewversion.ui \
    dlgrdrsetproperty.ui \
    dlgselectprojectsubset.ui \
    dlgfixfilepaths.ui \
    renamegroupsdialog.ui \
    dlgsetrdrpropertybyregexp.ui \
    userfitfunctionseditor.ui \
    dlgsetrdrpropertybyexpression.ui \
    dlgsetrdrpropertybyexpressioneditor.ui \
    dlgwelcomescreen.ui \
    dlgeditgroupandrole.ui

RESOURCES += quickfit3.qrc \
             ../extlibs/xits.qrc


TRANSLATIONS= ./translations/de.ts

INCLUDEPATH += ../../../../LIB/trunk/

win32 {
    RC_FILE = quickfit3.rc
}


!contains(QF3CONFIG,nowebkit) {
    QT+=webkit webkitwidgets
    DEFINES+=QF3_USE_WEBKIT
}

QT += core gui svg xml network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG +=  exceptions rtti stl link_prl

!release {
    CONFIG += console
}


macx{
  exists(qf3icon.icns) {
    ICON = qf3icon.icns
  }
}

ICON_FILES.files = qf3icon.icns\
                   qf3icon.ico\
                   qf3icon_old.ico
ICON_FILES.path = $${QFOUTPUT}


ASSETSSTYLESHEET_FILES.files = ./assets/stylesheets/*.*
ASSETSSTYLESHEET_FILES.path = $${QFOUTPUT}/assets/stylesheets/

ASSETSCOLORPALETTES_FILES.files = ./assets/colorpalettes/*.*
ASSETSCOLORPALETTES_FILES.path = $${QFOUTPUT}/assets/colorpalettes/

ASSETSFF_FILES.files = ./assets/userfitfunctions/*.*
ASSETSFF_FILES.path = $${QFOUTPUT}/assets/userfitfunctions/


HELPREF_FILES.files = ./help/parserreference/*.*
HELPREF_FILES.path = $${QFOUTPUT}/assets/help/parserreference/

HELPREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPREFPIC_FILES.path = $${QFOUTPUT}/assets/help/parserreference/pic/

HELPDLGREF_FILES.files = ./help/dlgsetrdrpropertybyexpression_ref/*.*
HELPDLGREF_FILES.path = $${QFOUTPUT}/assets/help/dlgsetrdrpropertybyexpression_ref/


HELPBayesPDF_FILES.files = ../doc/bayesian_fit/bayesian_fit.pdf
HELPBayesPDF_FILES.path = $${QFOUTPUT}/assets/help_pdf/


INSTALLS += ASSETSSTYLESHEET_FILES HELPREF_FILES HELPREFPIC_FILES ICON_FILES ASSETSFF_FILES ASSETSCOLORPALETTES_FILES HELPDLGREF_FILES HELPBayesPDF_FILES

OTHER_FILES += \
    releasenote.html



