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

    \note data is given as a list of columns
*/
QFLIB_EXPORT QString toMatlab(const QList<QList<double> >& data, bool noVarDef=false);
/*! \brief convert the given data columns as Matlab script
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QString toMatlab(const QList<QVector<double> >& data, bool noVarDef=false);
/*! \brief copy the given data columns as Matlab script to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void matlabCopy(const QList<QList<double> >& data);
/*! \brief copy the given data columns as Matlab script to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void matlabCopy(const QList<QVector<double> >& data);

/*! \brief convert the given data columns as Matlab script
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT QString toMatlab(const QList<QList<QVariant> >& data, bool noVarDef=false);
/*! \brief copy the given data columns as Matlab script to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void matlabCopy(const QList<QList<QVariant> >& data);
/*! \brief copy the given data columns as Matlab script to the clipboard
    \ingroup qf3lib_tools

    \note data is given as a list of columns
*/
QFLIB_EXPORT void matlabCopy(const QList<QVector<QVariant> >& data);

#endif // MATLABTOOLS_H
