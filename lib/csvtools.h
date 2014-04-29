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

    \note data is given as a list of columns
*/
QFLIB_EXPORT void csvCopy(const QList<QList<double> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList(), bool copyAllVariant=true);


/*! \brief copy the given data columns as CSV to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void csvCopy(const QList<QList<QVariant> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList(), bool copyAllVariant=true);

/*! \brief copy the given data columns as CSV to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void csvCopy(const QList<QVector<double> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList(), bool copyAllVariant=true);

/*! \brief exchange rows and columns
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QList<QVector<double> > csvDataRotate(const QList<QVector<double> >& data);
/*! \brief exchange rows and columns
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QList<QList<double> > csvDataRotate(const QList<QList<double> >& data);

/*! \brief exchange rows and columns
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QList<QList<QVariant> > csvDataRotate(const QList<QList<QVariant> >& data);

/*! \brief convert the given data to CSV
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QString toCSV(const QList<QVector<double> >& data, const QStringList& columnsNames, const QStringList& rowNames, QChar decimalSep='.', const QString colSep=QString(", "), bool withHeaders=true, QChar stringDelimiter=QLatin1Char('\"'), const QString& headerSep=QString("#! "), int precision=15);

/*! \brief convert the given data to CSV
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QString toCSV(const QList<QList<QVariant> >& data, const QStringList& columnsNames, const QStringList& rowNames, QChar decimalSep='.', const QString colSep=QString(", "), bool withHeaders=true, QChar stringDelimiter=QLatin1Char('\"'), const QString& headerSep=QString("#! "), int precision=15);

#endif // CSVTOOLS_H
