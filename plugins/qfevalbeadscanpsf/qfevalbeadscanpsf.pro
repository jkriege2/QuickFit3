#
#    Copyright (c) 2014 
#	
#	 last modification: $LastChangedDate: 2015-01-21 11:37:05 +0100 (Mi, 21 Jan 2015) $  (revision $Rev: 3738 $)
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

TARGET = eval_beadscanpsf
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./

include(../../qf3plugins.pri)
include(../../extlibs/tiff.pri)
include(../../extlibs/cimg.pri)
include(../../libquickfitwidgets.pri)
DESTDIR = $$QFOUTPUT/plugins

# Input
HEADERS += qfevalbeadscanpsf.h \
           qfevalbeadscanpsf_item.h \
           qfevalbeadscanpsf_editor.h \
    fitfuncpsfgaussian.h \
    qffitfunctionpsf3dgaussian.h \
    fitfuncpsfgaussianbeamwidth.h \
    fitfuncpsfgaussianbeamzintensity.h

SOURCES += qfevalbeadscanpsf.cpp \
           qfevalbeadscanpsf_item.cpp \
           qfevalbeadscanpsf_editor.cpp \
    fitfuncpsfgaussian.cpp \
    qffitfunctionpsf3dgaussian.cpp \
    fitfuncpsfgaussianbeamwidth.cpp \
    fitfuncpsfgaussianbeamzintensity.cpp

FORMS = qfevalbeadscanpsf_editor.ui   

RESOURCES += qfevalbeadscanpsf.qrc

TRANSLATIONS= ./translations/de.eval_beadscanpsf.ts

INCLUDEPATH += ../../lib/ \
               ../../libqf3widgets/


ASSETS_1color.files = ./examples/1color/*.*
ASSETS_1color.path = $${QFOUTPUT}/examples/$${TARGET}/1color/


ASSETS_2color.files = ./examples/2color/*.*
ASSETS_2color.path = $${QFOUTPUT}/examples/$${TARGET}/2color/


ASSETS_matlab.files = ./assets/matlab_script/*.*
ASSETS_matlab.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/matlab_script/


INSTALLS += ASSETS_1color ASSETS_2color ASSETS_matlab


include(../../quickfit3_configqtwidgets.pri)



