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

#ifndef QFGENERALFITFUNCTIONSPLUGIN_H
#define QFGENERALFITFUNCTIONSPLUGIN_H

#include "qfpluginfitfunction.h"

/*!
    \defgroup qf3fitfunp_fitfunction_general Fit Function Plugin
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for Fit Functions
    \ingroup qf3fitfunp_fitfunction_general
*/
class QFGeneralFitFunctionsPlugin : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
    public:

        /** Default constructor */
        QFGeneralFitFunctionsPlugin(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFGeneralFitFunctionsPlugin() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("General Fit Functions Plugin");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("implements several very common fit functions for QuickFit");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2013-2014 by Jan W. Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunction_general";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitfunc_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;
		
        QF_PLUGIN
    private:
};

#endif // QFGENERALFITFUNCTIONSPLUGIN_H
