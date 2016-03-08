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

DEFINES += QTLIB_IN_DLL IS_QUICKFIT3

MOC_DIR = ./.mocs/
UI_DIR = ./.uis/
RCC_DIR = ./.rccs/
OBJECTS_DIR = ./.objs/
GPL3LICENSE = ../doc/gpl-3.0.txt


include("autoversioning.pri")

include(quickfit_config.pri)

QFOUTPUT = $$DESTDIR
isEmpty( QFOUTPUT ) {
	QFOUTPUT = $$PWD/$${OUTPUTDIR_NAME}
}
message (Installing to $$QFOUTPUT)


HELP_FILES.files = ./help/*.*
HELP_FILES.path = $${QFOUTPUT}/assets/help/

HELPPIC_FILES.files = ./help/pic/*.*
HELPPIC_FILES.path = $${QFOUTPUT}/assets/help/pic/

ASSETS_FILES.files = ./assets/*.*
ASSETS_FILES.path = $${QFOUTPUT}/assets/

EXAMPLES_FILES.files = ./examples/*.*
EXAMPLES_FILES.path = $${QFOUTPUT}/examples/

ASSETS_TRANSLATIONS.files = ./translations/*.qm
ASSETS_TRANSLATIONS.path = $${QFOUTPUT}/assets/translations/

INSTALLS += HELP_FILES HELPPIC_FILES ASSETS_FILES ASSETS_TRANSLATIONS EXAMPLES_FILES

OTHER_FILES +=

include(quickfit3_configqtwidgets.pri)


