/*
Copyright (c) 2014
	
	last modification: $LastChangedDate: 2015-01-23 12:13:35 +0100 (Fr, 23 Jan 2015) $  (revision $Rev: 3746 $)

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


#ifndef QFPBasicImageExporters_H
#define QFPBasicImageExporters_H

#include "qfpluginexporters.h"
#include "qfpluginoptionsdialog.h"
#include "cimg.h"
/*!
    \defgroup qf3exporterplugins_exporters_basicimage Exporter Plugin
    \ingroup qf3exporterplugins
*/



/*! \brief plugin class for Exporters
    \ingroup qf3exporterplugins_exporters_basicimage
*/
class QFPBasicImageExporters : public QObject, public QFPluginExporters, public QFPluginOptionsDialogInterface {
        Q_OBJECT
        Q_INTERFACES(QFPluginOptionsDialogInterface)
        Q_INTERFACES(QFPluginExporters)
        Q_INTERFACES(QFPlugin)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "www.dkfz.de.QuickFit3.Plugins.QFPBasicImageExporters")
#endif	
    public:

        /** Default constructor */
        QFPBasicImageExporters(QObject* parent=NULL) ;
        /** Default destructor */
        virtual ~QFPBasicImageExporters() {}

        /** \brief name for the plugin */
        virtual QString getName() const {
            return tr("Basic image series exporters");
        };

        /** \brief short description for the plugin */
        virtual QString getDescription() const {
            return tr("This plugin provides common image series exporters (e.g. TIFF, RAW, ...)");
        };

        /** \brief author the plugin */
        virtual QString getAuthor() const {
            return QString("Jan W. Krieger");
        };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const {
            return tr("(c) 2015 by Jan W. Krieger");
        };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const {
            return tr("http://www.dkfz.de/Macromol/quickfit/");
        };

        /** \copydoc QFPlugin::getID()  */
        virtual QString getID() const {
            return "exporters_basicimage";
        };

        /** \copydoc QFPlugin::getIconFilename()  */
        virtual QString getIconFilename() const {
            // replace with your icon if you have one ... or use this default icon
            return ":/exporters_basicimages/icon.png";
        };

        /** \brief plugin version  */
        virtual void getVersion(int& major, int& minor) const {
            major=1;
            minor=1;
        };

        /** \brief return list of plugin IDs */
        virtual QStringList getIDs() const;

        /** \brief return a QFFitFunction instance for the given ID, created with the given parent object */
        virtual QFExporter* createExporter(QString id) const ;

        /** \brief return the display name for the options pane */
        QString pluginOptionsName() const;
        /** \brief return the display icon for the options pane */
        QIcon pluginOptionsIcon() const;
        /** \brief create a widget to display in the options dialog */
        QFPluginOptionsWidget* createOptionsWidget(QWidget* parent) ;

		QF_PLUGIN
    private:
};

#endif // QFPBasicImageExporters_H
