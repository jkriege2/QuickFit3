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

#ifndef FCS_FITFUCTIONS_H
#define FCS_FITFUCTIONS_H

#include "qfpluginfitfunction.h"

/*!
    \defgroup qf3fitfunp_fcs FCS Fitting Function Plugin
    \ingroup qf3fitfunplugins
*/







/*! \brief plugin class for FCS Fitting Functions
    \ingroup qf3fitfunp_fcs
*/
class QFPFitFunctionFCS : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFPFitFunctionFCS")
#endif


    public:
        QF_PLUGIN

        /** Default constructor */
        QFPFitFunctionFCS(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFPFitFunctionFCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("FCS fit functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("a basic set of FCS fit functions");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2010-2014 by Jan Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "qffcsfitfuncs";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            return ":/plugin_fitfunc_incon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=4;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;
    private:
};

#endif // FCS_FITFUCTIONS_H
