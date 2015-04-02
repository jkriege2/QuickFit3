/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFEXTENSIONMANAGER_H
#define QFEXTENSIONMANAGER_H

#include "lib_imexport.h"
#include <QObject>
#include <QMap>
#include <QWidget>
#include <QString>
#include "qfproject.h"
#include "programoptions.h"
#include "qfpluginservices.h"

// FORWARD DECLARATIONS
class QFExtension; // forward

/*! \brief manager class for QFExtension plugins
    \ingroup qf3extensionplugins
*/
class QFLIB_EXPORT QFExtensionManager : public QObject {
        Q_OBJECT
    public:
        /** \brief class constructor */
        QFExtensionManager(ProgramOptions* options, QObject* parent=NULL);
        /** \brief class destructor */
        virtual ~QFExtensionManager();



        /** \brief distribute objects to plugins that allow interaction with the main application */
        void distribute(QFProject* project);

        /*! \brief initialize the plugins */
        void init(QFPluginServices* services, QWidget* parentWidget);

        /*! \brief deinitialize the plugins */
        void deinit();

        /*! \brief read settings in the plugins */
        void readPluginSettings(ProgramOptions* settings);

        /*! \brief write settings in the plugins */
        void writePluginSettings(ProgramOptions* settings);

        /** \brief returns a list of the IDs of all available QFExtensions. */
        inline QStringList getIDList() { return items.keys(); }

        /** \brief returns the description for a specified QFExtension ID. */
        QString getDescription(QString ID);

        /** \brief returns the name for a specified QFExtension ID. */
        QString getName(QString ID);


        /** \brief returns the author for a specified QFExtension ID. */
        QString getAuthor(QString ID);

        /** \brief returns the copyright for a specified QFExtension ID. */
        QString getCopyright(QString ID);
        /** \brief returns the weblink for a specified QFExtension ID. */
        QString getWeblink(QString ID);
        /** \brief returns the icon filename for a specified QFExtension ID. */
        QString getIconFilename(QString ID);
        /** \brief returns the plugins filename for a specified QFExtension ID. */
        QString getPluginFilename(QString ID);
        /** \brief get plugins major version number */
        int getMajorVersion(QString id);
        /** \brief get plugins minor version number */
        int getMinorVersion(QString id);

        /** \brief return a pointer to the given QFExtension instance */
        QFExtension* getInstance(QString id);
        /** \brief return a pointer to the given Object instance */
        QObject* getQObjectInstance(QString id);
        /** \brief return \c true if a plugin with the given ID is registered */
        bool contains(QString ID);

        /** \brief returns the plugins main help file (html) for a specified QFExtension ID. */
        QString getPluginHelp(QString ID);
        /** \brief returns the plugins tutorial file (html) for a specified QFExtension ID. */
        QString getPluginTutorialMain(QString ID);
        /** \brief returns the plugins FAQ file (html) for a specified  ID. */
        QString getPluginFAQ(QString ID);
        /** \brief returns lists of titles (names) and links to all tutorials for for a specified plugin ID. */
        void getPluginTutorials(const QString& ID, QStringList& names, QStringList& files);
        /** \brief returns the plugins settings file (html) for a specified QFExtension ID. */
        QString getPluginSettings(QString ID);
        /** \brief returns the plugins copyright file (html) for a specified QFExtension ID. */
        QString getPluginCopyrightFile(QString ID);

        /** \brief returns a pointer to the instance of the main manager object (singleton within a QuickFit3 instance) */
        static QFExtensionManager* getInstance() {
            if (QFPluginServices::getInstance()) {
                return QFPluginServices::getInstance()->getExtensionManager();
            }
            return NULL;
        }
        void finalizePluginSearch();

        bool registerPlugin(const QString &filename_in, QObject *plugin, QFPluginHelpData &helpdata);

        /** \brief add a plugin from an external source */
        void addExtensionPlugin(const QString& filename, QFExtension* record );
        /** \brief add a plugin from an external source */
        void addExtensionPlugins(const QString& filename, QList<QFExtension*> records );

    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);
        void incProgress();
    protected:
        /** \brief this map is used to internally manage the available QFExtension s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QFExtension*> items;
        /** \brief this map is used to internally manage the available QFExtension s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QObject*> itemobjects;

        /** \brief this map contains the filenames of the plugin DLLs */
        QMap<QString, QString> filenames;

        ProgramOptions* m_options;

};

#endif // QFEXTENSIONMANAGER_H
