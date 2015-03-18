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

#include "qfpluginfitfunction.h"

/*!
    \defgroup doxygen_GROUPNAME Fit Function Plugin
    \ingroup qf3fitfunplugins
*/



/*! \brief plugin class for Fit Functions
    \ingroup doxygen_GROUPNAME
*/
class QFFitAlgorithmInst : public QObject, public QFPluginFitFunction {
        Q_OBJECT
        Q_INTERFACES(QFPluginFitFunction)
        Q_INTERFACES(QFPlugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFFitAlgorithmInst")
#endif	
    public:

        /** Default constructor */
        QFFitAlgorithmInst(QObject* parent=NULL):QObject() {}
        /** Default destructor */
        virtual ~QFFitAlgorithmInst() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return QObject::tr("");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return QObject::tr("");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QObject::tr("");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return QObject::tr("");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return QObject::tr("");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "target_id";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/lib/fitfunc_icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=0;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFFitFunction* get(QString id, QObject* parent) const ;
		
        QF_PLUGIN
    private:
};

#endif // HEADER_H
