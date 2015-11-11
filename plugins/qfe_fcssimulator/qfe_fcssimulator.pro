#
#    Copyright (c) 2014 
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

TARGET = qfe_fcssimulator
DEFINES += TARGETNAME=$$TARGET
DEPENDPATH += ./


include(../../qf3plugins.pri)
include(../../libquickfitwidgets.pri)
DESTDIR = $${QFOUTPUT}/plugins/

# Input
HEADERS += qfe_fcssimulator.h \
    qfefcssimmainwidnow.h \
    qfefcssimscripttab.h

SOURCES += qfe_fcssimulator.cpp \
    qfefcssimmainwidnow.cpp \
    qfefcssimscripttab.cpp

FORMS = \
    qfefcssimmainwidnow.ui \
    qfefcssimscripttab.ui

RESOURCES += qfe_fcssimulator.qrc

TRANSLATIONS= ./translations/de.qfe_fcssimulator.ts


include(../../quickfit3_configqtwidgets.pri)

spectradir=$$PWD/../../extlibsb040/FCSSimulator/spectra
spectradir_out=$${QFOUTPUT}/assets/plugins/$${TARGET}/
exampledir=$$PWD/../../extlibsb040/FCSSimulator/example_configs
exampledir_out=$${QFOUTPUT}/examples/$${TARGET}/
manualpdf=$$PWD/../../extlibsb040/FCSSimulator/manual/manual.pdf
manualpdf_out=$${QFOUTPUT}/assets/plugins/help/$${TARGET}/
win32|win64 {
    fcssim_exe=$$PWD/../../extlibsb040/FCSSimulator/diffusion4.exe
    fcssim_exe_out=$${QFOUTPUT}/diffusion4.exe
} else {
    macx {
        fcssim_exe=$$PWD/../../extlibsb040/FCSSimulator/diffusion4$${EXE_SUFFIX}
        fcssim_exe_out=$${QFOUTPUT}/assets/plugins/$${TARGET}/diffusion4$${EXE_SUFFIX}
    } else {
        fcssim_exe=$$PWD/../../extlibsb040/FCSSimulator/diffusion4$${EXE_SUFFIX}
        fcssim_exe_out=$${QFOUTPUT}/diffusion4$${EXE_SUFFIX}
    }
}

fcssim_clean.depends =
fcssim_clean.commands = $(MAKE) -C $$PWD/../../extlibsb040/FCSSimulator -f Makefile clean
fcssim_build.depends = fcssim_clean
fcssim_build.commands = $(MAKE) -C $$PWD/../../extlibsb040/FCSSimulator -f Makefile QuickFit
fcssim_copy.depends = fcssim_build
fcssim_copy.commands = $(COPY) $${fcssim_exe} $${fcssim_exe_out}
fcssim_spectracopy.depends = fcssim_build
fcssim_spectracopy.commands = $(MKDIR) $${spectradir_out} && $(COPY_DIR) $${spectradir} $${spectradir_out}
fcssim_examplecopy.depends = fcssim_build
fcssim_examplecopy.commands = $(MKDIR) $${exampledir_out} && $(COPY_DIR) $${exampledir} $${exampledir_out}
fcssim_manualcopy.depends = fcssim_build
fcssim_manualcopy.commands = $(MKDIR) $${manualpdf_out} && $(COPY_DIR) $${manualpdf} $${manualpdf_out}
QMAKE_EXTRA_TARGETS += fcssim_build fcssim_copy fcssim_spectracopy fcssim_examplecopy fcssim_manualcopy fcssim_clean
PRE_TARGETDEPS += fcssim_copy fcssim_spectracopy fcssim_examplecopy fcssim_manualcopy


ASSETSTEMPLATESDIFF4_FILES.files = ./assets/template_configs/*.*
ASSETSTEMPLATESDIFF4_FILES.path = $${QFOUTPUT}/assets/plugins/$${TARGET}/template_configs/

INSTALLS +=  ASSETSTEMPLATESDIFF4_FILES
