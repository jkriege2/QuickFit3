#ifndef CSVTOOLS_H
#define CSVTOOLS_H

#include <QString>
#include <QList>
#include <QVector>
#include <QTextStream>
#include "lib_imexport.h"

/*! \brief Reads in one line of text from the specified file and interpret it as Comma-Separated-Values file.
           Returns its contents as a vector of doubles.

 */
QFLIB_EXPORT QVector<double> csvReadline(QTextStream& f, QChar separator_char=',', QChar comment_char='#', double non_value=0);

#endif // CSVTOOLS_H
