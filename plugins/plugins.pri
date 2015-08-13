
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

#message("!!! !!! NOW IN plugins.pri PWD=$$PWD")

include($$PWD/../quickfit_config.pri)


INCLUDEPATH += $$PWD/../lib/ \
               $$PWD/../libqf3widgets/ \
               $$PWD/interfaces \
               $$PWD/base_classes \
               $$PWD/../extlibs/ \
               $$PWD/../extlibs/StatisticsTools


MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
QFOUTPUT = $$PWD/../$${OUTPUTDIR_NAME}
DESTDIR = $$QFOUTPUT/plugins

GPL3LICENSE = $$PWD/../doc/gpl-3.0.txt \
              $$PWD/../doc/lgpl-2.1.txt
LGPL2LICENSE = $$PWD/../doc/lgpl-2.1.txt

#release {
#    QFOUTPUT = ../../output_release
#} else {
#    #MOC_DIR = ./.mocsd/
#    #UI_DIR = ./.uisd/
#    #RCC_DIR = ./.rccsd/
#    #OBJECTS_DIR = ./.objsd/
#    QFOUTPUT = ../../output
#}



SRC_DISTRIBUTED.files =
SRC_DISTRIBUTED.path = $${QFOUTPUT}/source/plugins/$${TARGET}/




HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/

HELPPARSERREF_FILES.files = ./help/parserreference/*.*
HELPPARSERREF_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/

HELPPARSERREFPIC_FILES.files = ./help/parserreference/pic/*.*
HELPPARSERREFPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/parserreference/pic/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/plugins/help/$${TARGET}/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}

GLOBALCONFIG_FILES.files = ./globalconfig/*.*
GLOBALCONFIG_FILES.path = $${QFOUTPUT}/globalconfig_templates/

GLOBALCONFIGWIN32_FILES.files = ./globalconfig/win32/*.*
GLOBALCONFIGWIN32_FILES.path = $${QFOUTPUT}/globalconfig_templates/

GLOBALCONFIGLINUX_FILES.files = ./globalconfig/linux/*.*
GLOBALCONFIGLINUX_FILES.path = $${QFOUTPUT}/globalconfig_templates/

GLOBALCONFIGMACX_FILES.files = ./globalconfig/macx/*.*
GLOBALCONFIGMACX_FILES.path = $${QFOUTPUT}/globalconfig_templates/

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

EXAMPLES_FILES.files = ./examples/*.*
EXAMPLES_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/

EXAMPLESTESTDATA_FILES.files = ./examples/testdata/*.*
EXAMPLESTESTDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/testdata/
EXAMPLESTEST_DATA_FILES.files = ./examples/test_data/*.*
EXAMPLESTEST_DATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/test_data/
EXAMPLESDATA_FILES.files = ./examples/data/*.*
EXAMPLESDATA_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/data/
EXAMPLESRESULTS_FILES.files = ./examples/results/*.*
EXAMPLESRESULTS_FILES.path = $${QFOUTPUT}/examples/$${TARGET}/results/

INSTALLS += HELP_FILES \
            HELPPIC_FILES \
            ASSETS_FILES \
            GLOBALCONFIG_FILES \
            ASSETS_TRANSLATIONS \
            SRC_DISTRIBUTED \
            EXAMPLES_FILES \
            HELPPARSERREF_FILES \
            HELPPARSERREFPIC_FILES \
            EXAMPLESTESTDATA_FILES \
            EXAMPLESTEST_DATA_FILES \
            EXAMPLESDATA_FILES \
            EXAMPLESRESULTS_FILES

win32 {
    INSTALLS += GLOBALCONFIGWIN32_FILES
} else {
    macx {
        INSTALLS += GLOBALCONFIGMACX_FILES
    } else {
        INSTALLS += GLOBALCONFIGLINUX_FILES
    }
}

release {
    message("building $$TARGET $$TEMPLATE (plugin) in RELEASE mode, output is in $$QFOUTPUT")
} else {
    message("building $$TARGET $$TEMPLATE (plugin) in DEBUG mode, output is in $$QFOUTPUT")
}

DEFINES += QTLIB_IN_DLL LIB_IN_DLL IS_QUICKFIT3 IS_QUICKFIT3_PLUGIN

include($$PWD/../quickfit3_configqt.pri)


CONFIG += link_prl


include($$PWD/../libquickfit.pri)




#message("!!! !!! NOW LEAVING plugins.pri QFOUTPUT=$$QFOUTPUT")



