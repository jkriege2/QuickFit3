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

#ifndef QFRAWDATARECORDFACTORY_H
#define QFRAWDATARECORDFACTORY_H

#include <QObject>
#include <QMap>
#include <QProgressBar>
#include <QStatusBar>
#include <QWidget>
#include <QMenu>
#include <QPair>
#include <QList>
#include <QAction>
#include "programoptions.h"
#include "qfpluginrawdata.h"
#include "qfpluginservices.h"
#include "lib_imexport.h"

class QFRawDataRecord;

/*! \brief this class is a factory class for QFRawDataRecord objects. You can
           request an object of a specific type by calling createRecord() with
           the appropriate type string as argument
    \ingroup qf3lib_project

*/
class QFLIB_EXPORT QFRawDataRecordFactory : public QObject {
        Q_OBJECT
    public:

    protected:
        /** \brief this map is used to internally manage the available QFRawDataRecord s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QFPluginRawDataRecord*> items;

        /** \brief this map contains the filenames of the plugin DLLs */
        QMap<QString, QString> filenames;

        ProgramOptions* m_options;

    public:
        /** Default constructor */
        QFRawDataRecordFactory(ProgramOptions* options, QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRawDataRecordFactory();

        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory, QFPluginHelpData& helpdata);

        /** \brief distribute objects to plugins that allow interaction with the main application */
        void distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent);

        /** \brief deinitialize all plugins */
        void deinit();
        /** \brief deinitialize all plugins */
        void init();


        /** \brief returns a list of the IDs of all available QFRawDataRecords. */
        QStringList getIDList();

        /** \brief returns the description for a specified QFRawDataRecord ID. */
        QString getDescription(QString ID);

        /** \brief returns the name for a specified QFRawDataRecord ID. */
        QString getName(QString ID);

        /** \brief return \c true if a plugin with the given ID is registered */
        bool contains(QString ID);

        /** \brief returns a pointer to the given plugin or NULL */
        QFPluginRawDataRecord* getPlugin(const QString& ID);


        /** \brief returns the author for a specified QFRawDataRecord ID. */
        QString getAuthor(QString ID);

        /** \brief returns the copyright for a specified QFRawDataRecord ID. */
        QString getCopyright(QString ID);
        /** \brief returns the weblink for a specified QFRawDataRecord ID. */
        QString getWeblink(QString ID);
        /** \brief returns the icon filename for a specified QFRawDataRecord ID. */
        QString getIconFilename(QString ID);
        /** \brief returns the plugins filename for a specified QFRawDataRecord ID. */
        QString getPluginFilename(QString ID);

        /** \brief returns the plugins main help file (html) for a specified QFRawDataRecord ID. */
        QString getPluginHelp(QString ID);
        /** \brief returns the plugins tutorial file (html) for a specified QFRawDataRecord ID. */
        QString getPluginTutorialMain(QString ID);
        /** \brief returns the plugins FAQ file (html) for a specified  ID. */
        QString getPluginFAQ(QString ID);

        /** \brief returns lists of titles (names) and links to all tutorials for for a specified QFRawDataRecord ID. */
        void getPluginTutorials(const QString& ID, QStringList &names, QStringList& files);
        /** \brief returns the plugins settings file (html) for a specified QFRawDataRecord ID. */
        QString getPluginSettings(QString ID);
        /** \brief returns the plugins copyright file (html) for a specified QFRawDataRecord ID. */
        QString getPluginCopyrightFile(QString ID);

        /** \brief get plugins major version number */
        int getMajorVersion(QString id);
        /** \brief get plugins minor version number */
        int getMinorVersion(QString id);



        bool registerPlugin(const QString &filename_in, QObject *plugin, QFPluginHelpData &helpdata);

        /** \brief returns a new object (created by new) for a specified QFRawDataRecord ID. */
        QFRawDataRecord* createRecord(QString ID, QFProject* parent);

        /** \brief register menu items for specified ID. */
        void registerMenu(QString ID, QMenu* menu);


        /** \brief add a plugin from an external source */
        void addRDRPlugin(const QString& filename, QFPluginRawDataRecord* record );
        /** \brief add a plugin from an external source */
        void addRDRPlugins(const QString& filename, QList<QFPluginRawDataRecord*> records );

        void finalizePluginSearch();
    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);
    private:
};



#endif // QFRAWDATARECORDFACTORY_H
