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

TEMPLATE = lib
CONFIG += plugin

TARGET = qfe_helpeditor
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../plugins.pri)

include(../../libquickfitwidgets.pri)

DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfehelpeditor.h \
           qfehelpeditorwidget.h \
    replacedialog.h \
    finddialog.h \
    pasteimagedlg.h \
    selectresourceimage.h \
    pluginlinkdialog.h \
    subpluginlinkdialog.h \
    faqentrydialog.h \
    functionreferencedialog.h \
    newtabledialog.h

SOURCES += qfehelpeditor.cpp \
           qfehelpeditorwidget.cpp \
    replacedialog.cpp \
    finddialog.cpp \
    pasteimagedlg.cpp \
    selectresourceimage.cpp \
    pluginlinkdialog.cpp \
    subpluginlinkdialog.cpp \
    faqentrydialog.cpp \
    functionreferencedialog.cpp \
    newtabledialog.cpp

FORMS =    qfehelpeditorwidget.ui \
    replacedialog.ui \
    finddialog.ui \
    pasteimagedlg.ui \
    selectresourceimage.ui \
    pluginlinkdialog.ui \
    subpluginlinkdialog.ui \
    faqentrydialog.ui \
    functionreferencedialog.ui \
    newtabledialog.ui

RESOURCES += qfehelpeditor.qrc

TRANSLATIONS= ./translations/de.qfe_helpeditor.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/ \
               ../../../../../LIB/trunk/ \
               ../../../../../LIB/trunk/qt/

ASSETSe_FILES.files = ./assets/examples/*.*
ASSETSe_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/examples

ASSETSt_FILES.files = ./assets/templates/*.*
ASSETSt_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/templates

INSTALLS += ASSETSe_FILES ASSETSt_FILES

QT += gui xml svg
CONFIG += exceptions rtti stl



