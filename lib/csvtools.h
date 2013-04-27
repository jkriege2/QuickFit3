#ifndef CSVTOOLS_H
#define CSVTOOLS_H

#include <QString>
#include <QList>
#include <QVector>
#include <QTextStream>
#include <QStringList>
#include "lib_imexport.h"

/*! \brief Reads in one line of text from the specified file and interpret it as Comma-Separated-Values file.
           Returns its contents as a vector of doubles.
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QVector<double> csvReadline(QTextStream& f, QChar separator_char=',', QChar comment_char='#', double non_value=0, const QString& eolChars=QString("\n"), const QString& ignoreChars=QString("\r"));


/*! \brief copy the given data columns as CSV to the clipboard
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT void csvCopy(const QList<QList<double> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList());
/*! \brief copy the given data columns as CSV to the clipboard
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT void csvCopy(const QList<QVector<double> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList());

/*! \brief exchange rows and columns
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QList<QVector<double> > csvDataRotate(const QList<QVector<double> >& data);
/*! \brief exchange rows and columns
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QList<QList<double> > csvDataRotate(const QList<QList<double> >& data);

#endif // CSVTOOLS_H
