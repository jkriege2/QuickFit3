#ifndef MATLABTOOLS_H
#define MATLABTOOLS_H

#include <QString>
#include <QList>
#include <QVector>
#include <QTextStream>
#include <QStringList>
#include "lib_imexport.h"

/*! \brief convert the given data columns as Matlab script
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString toMatlab(const QList<QList<double> >& data, bool noVarDef=false);
/*! \brief copy the given data columns as Matlab script to the clipboard
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT void matlabCopy(const QList<QList<double> >& data);
/*! \brief copy the given data columns as Matlab script to the clipboard
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT void matlabCopy(const QList<QVector<double> >& data);

/*! \brief convert the given data columns as Matlab script
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT QString toMatlab(const QList<QList<QVariant> >& data, bool noVarDef=false);
/*! \brief copy the given data columns as Matlab script to the clipboard
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT void matlabCopy(const QList<QList<QVariant> >& data);
/*! \brief copy the given data columns as Matlab script to the clipboard
    \ingroup qf3lib_tools
*/
QFLIB_EXPORT void matlabCopy(const QList<QVector<QVariant> >& data);

#endif // MATLABTOOLS_H
