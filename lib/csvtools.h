/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef CSVTOOLS_H
#define CSVTOOLS_H

#include <QString>
#include <QList>
#include <QVector>
#include <QTextStream>
#include <QStringList>
#include "lib_imexport.h"

/*! \brief Reads in one line of text from the specified stream and interpret it as Comma-Separated-Values file.
           Returns its contents as a vector of doubles.
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QVector<double> csvReadline(QTextStream& f, QChar separator_char=',', QChar comment_char='#', double non_value=0, const QString& eolChars=QString("\n"), const QString& ignoreChars=QString("\r"));
/*! \brief Reads in one line of text from the specified string and interpret it as Comma-Separated-Values file.
           Returns its contents as a vector of doubles.
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QVector<double> csvReadline(QString f, QChar separator_char=',', QChar comment_char='#', double non_value=0, const QString& eolChars=QString("\n"), const QString& ignoreChars=QString("\r"));

/*! \brief Reads a CSV file
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QList<QVector<double> > readCSV(QTextStream& f, QChar separator_char=',', QChar decimalsep_char='.', QChar comment_char='#', const QString& headerComment=QString("#!"), double non_value=0, const QString& eolChars=QString("\n"), const QString& ignoreChars=QString("\r"), QStringList* colNames=NULL);

/*! \brief Reads a CSV file and ask user to give file parameters
    \ingroup qf3lib_tools

 */
QFLIB_EXPORT QList<QVector<double> > importCSVAskUser(const QString& filename, const QString& configprefix=QString("import_csv/"), QStringList* colNames=NULL);

/*! \brief copy the given data columns as CSV to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void csvCopy(const QList<QList<double> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList(), bool withLocalePriority=true);


/*! \brief copy the given data columns as CSV to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void csvCopy(const QList<QList<QVariant> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList(), bool withLocalePriority=true);

/*! \brief copy the given data columns as CSV to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void csvCopy(const QList<QVector<double> >& data, const QStringList& columnsNames=QStringList(), const QStringList& rowNames=QStringList(), bool withLocalePriority=true);

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
QFLIB_EXPORT QString toCSV(const QList<QVector<double> >& data, const QStringList& columnsNames, const QStringList& rowNames, QChar decimalSep='.', const QString colSep=QString(", "), bool withHeaders=true, QChar stringDelimiter=QLatin1Char('\"'), const QString& headerSep=QString("#! "), int precision=15, int fieldWidth=-1);

/*! \brief convert the given data to CSV
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QString toCSV(const QList<QList<QVariant> >& data, const QStringList& columnsNames, const QStringList& rowNames, QChar decimalSep='.', const QString colSep=QString(", "), bool withHeaders=true, QChar stringDelimiter=QLatin1Char('\"'), const QString& headerSep=QString("#! "), int precision=15, int fieldWidth=-1);

/*! \brief true to guess the CSV parameters in a given string data
    \ingroup qf3lib_tools

*/
QFLIB_EXPORT bool guessCSVParameters( QString data, char* out_sep, char* out_dec=NULL, char* out_comment=NULL, QString* out_headercomment=NULL);
#endif // CSVTOOLS_H
