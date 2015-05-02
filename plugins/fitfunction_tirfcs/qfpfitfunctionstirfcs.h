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

#ifndef QFPFITFUNCTIONSTIRFCS_H
#define QFPFITFUNCTIONSTIRFCS_H

#include "qfpluginfitfunction.h"
#include <QStringList>
#include "qffitfunctionconfigforglobalfitinterface.h"
/*!
    \defgroup qf3fitfunp_fitfunctions_spimfcs TIR-FCS Fit Functions
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin TIR-FCS Fit Functions
    \ingroup qf3fitfunp_fitfunctions_spimfcs
*/
class QFPFitFunctionsTIRFCS : public QObject, public QFPluginFitFunction, public QFFitFunctionConfigForGlobalFitInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction QFFitFunctionConfigForGlobalFitInterface)
        Q_INTERFACES(QFPlugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFPFitFunctionsTIRFCS")
#endif

    public:
        QF_PLUGIN


        /** Default constructor */
        QFPFitFunctionsTIRFCS(QObject* parent=NULL): QObject(parent) {}
        /** Default destructor */
        virtual ~QFPFitFunctionsTIRFCS() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("TIR-FCS/TIR-FCCS Fit Functions");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin implements a set of fit functions used for imaging FCS/FCCS using the TIR imaging mode");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2013-2015 by Jan W. Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "fitfunctions_tirfcs";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitfunc_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=2;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(const QString& id) const ;

        int getGlobalFitConfigCount() const;
        GlobalFitConfig getGlobalFitConfig(int i) const;

    private:
};

#endif // QFPFITFUNCTIONSTIRFCS_H
