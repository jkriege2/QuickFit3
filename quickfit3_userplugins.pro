
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

# NOTE: This shouldn't be built as a SHADOW build
!equals(PWD, $${OUT_PWD}) {
    warning(This project should not be built as a shadow build!!!)
}

include("quickfit3.pri")


CONFIG(release, debug|release):message("building QuickFit in RELEASE mode")
CONFIG(debug, debug|release):message("building QuickFit in DEBUG mode")


TEMPLATE = subdirs

# if you have custom plugins, add them to a file userplugins.inc:
# with syntax, e.g. as:
#
# SUBDIRS += plg_mypluginname
# plg_mypluginname.subdir=./plugins/myplugin
# plg_mypluginname.depends = lib libqf3widgets
exists(userplugins.inc):include(userplugins.inc)




RESOURCES += \
    ../lib/lib.qrc

DISTFILES += \
    quickfit3.supp

