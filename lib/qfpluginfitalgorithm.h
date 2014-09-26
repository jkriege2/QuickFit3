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

#ifndef QFPLUGINFITALGORITHM_H
#define QFPLUGINFITALGORITHM_H

#include <QtPlugin>
#include "lib_imexport.h"
#include <QString>
#include <QStringList>
#include <QObject>
#include "qffitalgorithm.h"
#include "qfplugin.h"

/** \brief virtual interface class for all QuickFit 3 fit algorithm plugins
    \ingroup qf3fitalgplugins

    These plugins may contain several fitting functions. The function getIDs() returns a QStringList
    with all implemented plugin IDs.
*/
class QFLIB_EXPORT QFPluginFitAlgorithm: public QFPlugin {
    public:
        /** Default destructor */
        virtual ~QFPluginFitAlgorithm() {}

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const =0;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(QString id, QObject* parent) const =0;

        /** \brief deinit plugin, this function is guaranteed to be called once, before  */
        virtual void deinit() {};

        /** \brief init plugin, this function is guaranteed to be called once, before  */
        virtual void init() {};
    protected:
    private:
};

Q_DECLARE_INTERFACE(QFPluginFitAlgorithm,
                     "www.dkfz.de.b040.quickfit3.QFPluginFitAlgorithm/1.0")

#endif // QFPLUGINFITALGORITHM_H
