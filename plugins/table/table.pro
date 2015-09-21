#
#    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg
#	
#	 
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

TEMPLATE = lib
CONFIG += plugin


TARGET = qfrdrtable
DEPENDPATH += ./

include(../../qf3plugins.pri)

DESTDIR = $$QFOUTPUT/plugins
include(../../libquickfitwidgets.pri)

# Input
HEADERS += qfrdrtable.h \
           qfrdrtableeditor.h \
           qfprdrtable.h \
    tableresizedialog.h \
    qfrdrtabledelegate.h \
    qfrdrploteditor.h \
    qfrdrtableplotwidget.h \
    qftablepluginmodel.h \
    qfrdrtablecolumnvaluesdialog.h \
    qfrdrtableformuladialog.h \
    qfrdrtablesortdialog.h \
    qfrdrtablehistogramdialog.h \
    qfrdrtableparserfunctions.h \
    qfrdrtableenhancedtableview.h \
    qftablegraphsettings.h \
    qfrdrtablecolumneditor.h \
    qfrdrtablecurvefitdialog.h \
    qfrdrtableregressiondialog.h \
    dlgcolorbarcoloring.h \
    qfrdrtable2dhistogramdialog.h \
    qfrdrtableplotsettingswidget.h \
    qfrdrtableaxissettingswidget.h \
    qfrdrtableindexedstatdialog.h \
    dlgimporttable.h \
    qfrdrtablemulticolumneditor.h \
    qfrdrtablesetcellsdialog.h \
    qfrdrtablecolumnsboxplotdialog.h

SOURCES += qfrdrtable.cpp \
           qfrdrtableeditor.cpp \
           qfprdrtable.cpp \
    tableresizedialog.cpp \
    qfrdrtabledelegate.cpp \
    qfrdrploteditor.cpp \
    qfrdrtableplotwidget.cpp \
    qftablepluginmodel.cpp \
    qfrdrtablecolumnvaluesdialog.cpp \
   qfrdrtableformuladialog.cpp \
    qfrdrtablesortdialog.cpp \
    qfrdrtablehistogramdialog.cpp \
    qfrdrtableparserfunctions.cpp \
    qfrdrtableenhancedtableview.cpp \
    qftablegraphsettings.cpp \
    qfrdrtablecolumneditor.cpp \
    qfrdrtablecurvefitdialog.cpp \
    qfrdrtableregressiondialog.cpp \
    dlgcolorbarcoloring.cpp \
    qfrdrtable2dhistogramdialog.cpp \
    qfrdrtableplotsettingswidget.cpp \
    qfrdrtableaxissettingswidget.cpp \
    qfrdrtableindexedstatdialog.cpp \
    dlgimporttable.cpp \
    qfrdrtablemulticolumneditor.cpp \
    qfrdrtablesetcellsdialog.cpp \
    qfrdrtablecolumnsboxplotdialog.cpp

FORMS = tableresizedialog.ui \
    qfrdrtableplotwidget.ui \
    qfrdrtablecolumnvaluesdialog.ui \
    qfrdrtableformuladialog.ui \
    qfrdrtablesortdialog.ui \
    qfrdrtablehistogramdialog.ui \
    qftablegraphsettings.ui \
    qfrdrtablecolumneditor.ui \
    qfrdrtablecurvefitdialog.ui \
    qfrdrtableregressiondialog.ui \
    dlgcolorbarcoloring.ui \
    qfrdrtable2dhistogramdialog.ui \
    qfrdrtableplotsettingswidget.ui \
    qfrdrtableaxissettingswidget.ui \
    qfrdrtableindexedstatdialog.ui \
    dlgimporttable.ui \
    qfrdrtablemulticolumneditor.ui \
    qfrdrtablesetcellsdialog.ui \
    qfrdrtablecolumnsboxplotdialog.ui

RESOURCES += qfrdrtable.qrc \
    ../../lib/lib.qrc

TRANSLATIONS= ./translations/de.qfrdrtable.ts

INCLUDEPATH +=

QT += gui xml
CONFIG += exceptions rtti stl



HELPREF_FILES.files = ./help/parserreference/*.*
HELPREF_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/

HELPREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPREFPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/pic/

EXAMPLESFCSTEST_DATA_FILES.files = ./examples/fcs_simdata/*.*
EXAMPLESFCSTEST_DATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/fcs_simdata/

INSTALLS += HELPREF_FILES HELPREFPIC_FILES EXAMPLESFCSTEST_DATA_FILES



