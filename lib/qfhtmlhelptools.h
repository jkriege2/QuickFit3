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

typedef QList<QPair<QString, QString> > QFHelpReplacesList;
/*! \copydoc QFPluginServices::transformQF3HelpHTML()
    \ingroup qf3lib_tools
 */
QFLIB_EXPORT QString transformQF3HelpHTML(const QString& input_html, const QString& filename, bool removeNonReplaced=true, const QFHelpReplacesList& more_replaces=QFHelpReplacesList(), bool insertTooltips=false, bool dontCreatePics=false);
/*! \brief transforms some special tags in the given input file. If the file does not exist, defaultText is used
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QString transformQF3HelpHTMLFile(const QString& filename, const QString& defaultText=QString("<center>No help available ...</center>"), bool removeNonReplaced=true, const QFHelpReplacesList& more_replaces=QFHelpReplacesList(), bool insertTooltips=false, bool dontCreatePics=false);


#endif // QFHTMLHELPTOOLS_H
