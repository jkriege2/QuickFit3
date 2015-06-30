/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
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
#include "qfpluginservices.h"

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
QFLIB_EXPORT QString transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced=true, const QFHelpReplacesList& more_replaces=QFHelpReplacesList(), bool insertTooltips=false, bool dontCreatePics=false, bool isMainHelp=false, const QString& texfilenameaddition=QString());
/*! \brief transforms some special tags in the given input file. If the file does not exist, defaultText is used
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QString transformQF3HelpHTMLFile(const QString& filename, const QString& defaultText=QString("<center>No help available ...</center>"), bool removeNonReplaced=true, const QFHelpReplacesList& more_replaces=QFHelpReplacesList(), bool insertTooltips=false, bool dontCreatePics=false, bool isMainHelp=false, const QString& texfilenameaddition=QString());



/*! \brief escape characters to HTML entities
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString escapeHTML(const QString& data);

/*! \brief deescape characters from HTML entities
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString deescapeHTML(const QString& data);


#ifndef QFMATHPARSER_MATHPARSERTEST
/*! \brief extract FAQs from a faq.html file
    \ingroup qf3lib_tools


 */
QFLIB_EXPORT void parseFAQ(const QString& filename, const QString& pluginID, QMap<QString, QFFAQData> &faqs);

QFLIB_EXPORT void parseTooltips(const QString& directory, QMap<QString, QFToolTipsData>& tooltips);
#endif

QFLIB_EXPORT void parseAutolinks(const QString& directory, QMap<QString, QString>& autolinks);

QFLIB_EXPORT void parseGlobalreplaces(const QString& directory);

#endif // QFHTMLHELPTOOLS_H
