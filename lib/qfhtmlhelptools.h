/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFHTMLHELPTOOLS_H
#define QFHTMLHELPTOOLS_H

#include "qfmathtools.h"

#include <cmath>
#include <cfloat>
#include <QSettings>
#include <QWidget>
#include <QSplitter>
#include <QFile>
#include "lib_imexport.h"
#include <QFileDialog>
#include <QRegExp>
#include <QAction>
#include <QToolButton>
#include <QString>
#include <QIcon>
#include <QListWidget>
#include "qtriple.h"
#include <QTextStream>

/*! \brief remove all HTML comments from a string
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString removeHTMLComments(const QString& data);

/*! \brief remove all HTML markup from a string
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString removeHTML(const QString& data);

/*! \brief extract the title of a HTML file
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString HTMLGetTitle(const QString& data);

typedef QList<QPair<QString, QString> > QFHelpReplacesList;
/*! \copydoc QFPluginServices::transformQF3HelpHTML()
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced=true, const QFHelpReplacesList& more_replaces=QFHelpReplacesList(), bool insertTooltips=false, bool dontCreatePics=false, bool isMainHelp=false);
/*! \brief transforms some special tags in the given input file. If the file does not exist, defaultText is used
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QString transformQF3HelpHTMLFile(const QString& filename, const QString& defaultText=QString("<center>No help available ...</center>"), bool removeNonReplaced=true, const QFHelpReplacesList& more_replaces=QFHelpReplacesList(), bool insertTooltips=false, bool dontCreatePics=false, bool isMainHelp=false);



/*! \brief escape characters to HTML entities
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString escapeHTML(const QString& data);

/*! \brief deescape characters from HTML entities
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString deescapeHTML(const QString& data);



#endif // QFHTMLHELPTOOLS_H
