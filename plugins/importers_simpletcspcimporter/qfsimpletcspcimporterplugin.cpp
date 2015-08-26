/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfsimpletcspcimporterplugin.h"
#include "qfsimpletcspcimporter.h"
#include "qfpluginservices.h"
#include "qfpicoquanttexttcspcimporter.h"


QStringList QFSimpleTCSPCImporterPlugin::getIDs() const {
    QStringList res;
    res<<"tcspcimporter_simpletcspc"<<"tcspcimporter_picoquanttexttcspc";
    return res;
}

QFImporter* QFSimpleTCSPCImporterPlugin::createImporter(QString id) const  {
    if (id=="tcspcimporter_simpletcspc") {
        return new QFSimpleTCSPCImporter();
    } else if (id=="tcspcimporter_picoquanttexttcspc") {
        return new QFPicoquantTextTCSPCImporter();
    }
    return NULL;
}

void QFSimpleTCSPCImporterPlugin::init()
{
    QFPluginServices::getInstance()->appendOrAddHTMLReplacement("FILEFORMATS_LIST", QString("<li><b>%2:</b><ul>\n"
                                                                                            "<li><a href=\"$$plugin_info:helpdir:%1$$/tcspcimporter_simpletcspc.html\">Simple TCSPC file [Hungarian 64-bit]</a></li>\n"
                                                                                            "</ul></li>\n").arg(getID()).arg(getName()));

}

Q_EXPORT_PLUGIN2(importers_simpletcspcimporter, QFSimpleTCSPCImporterPlugin)
