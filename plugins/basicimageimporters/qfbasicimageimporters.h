/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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


#ifndef QFBASICIMAGEIMPORTERS_H
#define QFBASICIMAGEIMPORTERS_H
#include "qfpluginservices.h"
#include "qfpluginimporters.h"
#include "qfimagereaderlibtiff.h"
#include "qfpluginoptionsdialog.h"
#include "qfimagereadertinytiff.h"
#include "qfimagereadercsv.h"

/*!
    \defgroup qf3importerplugins_importers_basicimages Importer Plugin
    \ingroup qf3importerplugins
*/



/*! \brief plugin class for Importers
    \ingroup qf3importerplugins_importers_basicimages
*/
class QFBasicImageImporters : public QObject, public QFPluginImporters, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginImporters QFPluginOptionsDialogInterface)
        Q_INTERFACES(QFPlugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFBasicImageImporters")
#endif
    public:

        /** Default constructor */
        QFBasicImageImporters(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFBasicImageImporters() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("Basic image series importers");
        }

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin provides a set of common image series importers to all other plugins in QuickFit");
        }

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan Krieger");
        }

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2012-2015 by Jan Krieger");
        }

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        }

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "importers_basicimages";
        }

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            return ":/importers_basicimages/icon.png";
        }

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=3;
        }

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFImporter* createImporter(QString id) const ;

        /** \brief return the display name for the options pane */
        QString pluginOptionsName() const;
        /** \brief return the display icon for the options pane */
        QIcon pluginOptionsIcon() const;
        /** \brief create a widget to display in the options dialog */
        QFPluginOptionsWidget* createOptionsWidget(QWidget* parent) ;

        QF_PLUGIN
    private:
};

#endif // QFBASICIMAGEIMPORTERS_H
