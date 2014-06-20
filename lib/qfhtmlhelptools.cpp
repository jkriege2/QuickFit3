/*
    Copyright (c) 2008-2014 Jan W. Krieger, German Cancer Research Center

    last modification: $LastChangedDate$  (revision $Rev$)

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

#include "qfhtmlhelptools.h"
#include "qfpluginservices.h"
#include "qfplugin.h"
#include "qffitalgorithm.h"
#include "qffitfunction.h"
#include "qffitalgorithmmanager.h"
#include "qffitfunctionmanager.h"


QString removeHTMLComments(const QString& data) {
     QRegExp rxComments("<!--(.*)-->", Qt::CaseInsensitive);
     rxComments.setMinimal(true);
     QString data1=data;
     data1.remove(rxComments);
     return data1;
}

QString transformQF3HelpHTMLFile(const QString& filename, const QString& defaultText, bool removeNonReplaced, const QFHelpReplacesList& more_replaces, bool insertTooltips, bool dontCreatePics, bool isMainHelp) {
    QFile f(filename);
    if (f.open(QIODevice::ReadOnly|QIODevice::Text)) {
        QTextStream in(&f);
        return transformQF3HelpHTML(in.readAll(), filename, removeNonReplaced, more_replaces, insertTooltips, dontCreatePics, isMainHelp);
    } else {
        return transformQF3HelpHTML(defaultText, filename, removeNonReplaced, more_replaces, insertTooltips, dontCreatePics, isMainHelp);
    }
}

QString transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced, const QList<QPair<QString, QString> >& more_replaces, bool insertTooltips, bool dontCreatePics, bool isMainHelp) {
    return QFPluginServices::getInstance()->transformQF3HelpHTML(input_html, filename, removeNonReplaced, more_replaces, insertTooltips,  dontCreatePics, isMainHelp);
}
