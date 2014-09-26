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

#ifndef QFPFITFUNCTIONSSPIMFCS_H
#define QFPFITFUNCTIONSSPIMFCS_H

#include "qfpluginfitfunction.h"
#include <QStringList>
#include "qffitfunctionconfigforglobalfitinterface.h"
/*!
    \defgroup qf3fitfunp_fitfunctions_spimfcs SPIM-FCS Fit Functions
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin SPIM-FCS Fit Functions
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFPFitFunctionsSPIMFCS : public QObject, public QFPluginFitFunction, public QFFitFunctionConfigForGlobalFitInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction QFFitFunctionConfigForGlobalFitInterface)
        Q_INTERFACES(QFPlugin)
    public:
        QF_PLUGIN


        /** Default constructor */
        QFPFitFunctionsSPIMFCS(QObject* parent=NULL) {}
        /** Default destructor */
        virtual ~QFPFitFunctionsSPIMFCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("SPIM-FCS/SPIM-FCCS: Fit Functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin implements a set of fit functions used for imaging FCS/FCCS using the SPIM imaging mode");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2011-2014 by Jan W. Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunctions_spimfcs";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitfunc_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=6;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;

        int getGlobalFitConfigCount() const;
        GlobalFitConfig getGlobalFitConfig(int i) const;

    private:
};

#endif // QFPFITFUNCTIONSSPIMFCS_H
