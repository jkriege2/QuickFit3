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

QT += core gui svg xml network opengl
#message("Qt Version: $$QT_MAJOR_VERSION . $$QT_MINOR_VERSION")
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
    #message("Qt Major Version >5, using special Qt5.x include syntax for widgets")
    win32{
        !contains(LIBS, -lopengl32): LIBS += -lopengl32
        !contains(LIBS, -lglu32): LIBS += -lglu32
    }
    greaterThan(QT_MINOR_VERSION, 3) {
        DEFINE += QFVisualize3DGLWidget_USES_QOPENGLWIDGET
    }
}
CONFIG += exceptions rtti stl link_prl
INCLUDEPATH += $$PWD/extlibsb040/JKQtPlotter/ \
               $$PWD/extlibsb040/StatisticsTools/
