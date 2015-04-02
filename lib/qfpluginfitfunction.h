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

#ifndef QFPLUGINFITFUNCTION_H
#define QFPLUGINFITFUNCTION_H

#include "lib_imexport.h"
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QObject>
#include "qffitfunction.h"
#include "qfplugin.h"


class QFLIB_EXPORT QFFitFunctionFactory {
    public:
        /** Default destructor */
        virtual ~QFFitFunctionFactory() {}

        /** \brief return list of plugin IDs */
        virtual QStringList fitFunctionFactoryGetIDs() const =0;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* fitFunctionFactoryGet(QString id, QObject* parent) const =0;

        virtual QString fitFunctionFactoryGetHelpFile(const QString& id) const =0;

};

Q_DECLARE_INTERFACE(QFFitFunctionFactory,
                     "www.dkfz.de.b040.quickfit3.QFFitFunctionFactory/1.0")

/** \brief virtual interface class for all QuickFit 3 fit function plugins
    \ingroup qf3fitfunplugins

    These plugins may contain several fitting functions. The function getIDs() returns a QStringList
    with all implemented plugin IDs.
*/
class QFLIB_EXPORT QFPluginFitFunction: public QFPlugin {
    public:
        /** Default destructor */
        virtual ~QFPluginFitFunction() {}

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const =0;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const =0;

        /** \brief deinit plugin, this function is guaranteed to be called once, before  */
        virtual void deinit(){};

        /** \brief init plugin, this function is guaranteed to be called once, before  */
        virtual void init(){};
};

Q_DECLARE_INTERFACE(QFPluginFitFunction,
                     "www.dkfz.de.b040.quickfit3.QFPluginFitFunction/2.0")


#endif // QFPLUGINFITFUNCTION_H
