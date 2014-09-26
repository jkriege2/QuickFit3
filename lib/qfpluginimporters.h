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

#ifndef QFPLUGINIMPORTERS_H
#define QFPLUGINIMPORTERS_H

#include "lib_imexport.h"
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QObject>
#include "qfimporter.h"
#include "qfplugin.h"

/** \brief virtual interface class for (all QuickFit 3 fit function plugins)file) importers in QuickFit 3.0
    \ingroup qf3importerplugins
*/
class QFLIB_EXPORT QFPluginImporters: public QFPlugin {
    public:
        /** Default destructor */
        virtual ~QFPluginImporters() {}

        /** \brief return the QFImporter's in this plugin */
        virtual QStringList getIDs() const =0;

        /** \brief return a QFImporter instance for the given ID */
        virtual QFImporter* createImporter(QString id) const =0;

        /** \brief deinit plugin, this function is guaranteed to be called once, before  */
        virtual void deinit() {};

        /** \brief init plugin, this function is guaranteed to be called once, before  */
        virtual void init() {};
};

Q_DECLARE_INTERFACE(QFPluginImporters,
                     "www.dkfz.de.b040.quickfit3.QFPluginImporters/1.0")


#endif // QFPLUGINIMPORTERS_H
