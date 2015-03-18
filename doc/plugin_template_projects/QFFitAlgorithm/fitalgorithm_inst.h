/*
Copyright (c) 2014
	
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


#ifndef HEADER_H
#define HEADER_H

#include "qfpluginfitalgorithm.h"

/*!
    \defgroup doxygen_GROUPNAME Levmar Fitting Algorithm Plugin
    \ingroup qf3fitalgplugins
*/






/*! \brief plugin class for Levmar Fit Algorithm
    \ingroup qf3fitalgp_levmar
*/
class QFFitAlgorithmInstPlugin : public QObject, public QFPluginFitAlgorithm {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitAlgorithm)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFFitAlgorithmInstPlugin")
#endif	
    public:
		QF_PLUGIN

        /** Default constructor */
        QFFitAlgorithmInstPlugin(QObject* parent=NULL): QObject(parent) {}
        /** Default destructor */
        virtual ~QFFitAlgorithmInstPlugin() {}



        /** \brief name for the plugin */
        virtual QString getName() const {
            return QObject::tr(/* plugin name */);
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return QObject::tr(/* plugin description */);
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString(/* plugin author */);
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return QObject::tr(/* plugin copyright */);
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return QObject::tr(/* plugin weblink */);
        };


        /** \brief plugin ID  */
        virtual QString getID() const {
            return "target_id";
        };

        /** \brief plugin icon  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitalg_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitAlgorithm* get(QString id, QObject* parent) const;
    private:
};

#endif // HEADER_H
