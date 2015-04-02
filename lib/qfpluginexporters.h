/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2014-09-26 12:42:55 +0200 (Fr, 26 Sep 2014) $  (revision $Rev: 3509 $)

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

#ifndef QFPluginExporters_H
#define QFPluginExporters_H

#include "lib_imexport.h"
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QObject>
#include "qfexporter.h"
#include "qfplugin.h"



/** \brief virtual interface class for (all QuickFit 3 fit function plugins)file) importers in QuickFit 3.0
    \ingroup qf3exporterplugins
*/
class QFLIB_EXPORT QFPluginExporters: public QFPlugin {
    public:
        /** Default destructor */
        virtual ~QFPluginExporters() {}

        /** \brief return the QFImporter's in this plugin */
        virtual QStringList getIDs() const =0;

        /** \brief return a QFImporter instance for the given ID */
        virtual QFExporter* createExporter(QString id) const =0;

        /** \brief deinit plugin, this function is guaranteed to be called once, before  */
        virtual void deinit() {};

        /** \brief init plugin, this function is guaranteed to be called once, before  */
        virtual void init() {};
};

Q_DECLARE_INTERFACE(QFPluginExporters,
                     "www.dkfz.de.b040.quickfit3.QFPluginExporters/1.0")


#endif // QFPluginExporters_H
