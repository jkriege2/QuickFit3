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

#ifndef QFIMPORTERMANAGER_H
#define QFIMPORTERMANAGER_H

#include <QObject>
#include "lib_imexport.h"
#include <QObject>
#include <QMap>
#include <QWidget>
#include <QString>
#include "qfproject.h"
#include "programoptions.h"
#include "qfimporter.h"
#include "qfpluginimporters.h"
#include "qfpluginservices.h"

/*! \brief manager class for QFImporter plugins
    \ingroup qf3importerplugins
*/
class QFLIB_EXPORT QFImporterManager : public QObject {
        Q_OBJECT
    public:
        /** \brief class constructor */
        explicit QFImporterManager(ProgramOptions* options, QObject *parent = 0);


        /** \brief return a list of all implemented IDs */
        QStringList getIDList(int i) const ;

        /** \brief return number of available plugins */
        int pluginCount() { return plugins.size(); }

        /** \brief name for the plugin */
        virtual QString getName(int i) const;

        /** \brief plugin filename */
        virtual QString getPluginFilename(int i) const;

        /** \brief short description for the plugin */
        virtual QString getDescription(int i) const;

        /** \brief plugin filename */
        virtual QString getIconFilename(int i) const;

        /** \brief author the plugin */
        virtual QString getAuthor(int i) const;

        /** \brief ID of the plugin */
        virtual QString getID(int i) const;

        /** \brief copyright information the plugin */
        virtual QString getCopyright(int i) const;

        /** \brief weblink for the plugin */
        virtual QString getWeblink(int i) const;

        /** \brief returns the plugins main help file (html) for a specified  ID. */
        QString getPluginHelp(int ID);
        /** \brief returns the plugins tutorial file (html) for a specified  ID. */
        QString getPluginTutorialMain(int ID);
        /** \brief returns the plugins FAQ file (html) for a specified  ID. */
        QString getPluginFAQ(int ID);
        /** \brief returns lists of titles (names) and links to all tutorials for for a specified plugin ID. */
        void getPluginTutorials(int ID, QStringList& names, QStringList& files);
        /** \brief returns the plugins tutorial file (html) for a specified  ID. */
        QString getPluginSettings(int ID);
        /** \brief returns the plugins copyright file (html) for a specified  ID. */
        QString getPluginCopyrightFile(int ID);
        /** \brief returns a fit algorthms \a faID main help file (html) for a specified plugin \a ID. */
        QString getPluginHelp(int ID, QString faID);

        QFImporter* createImporter(const QString& id) const;

        /** \brief return a list of the models which may be converted to the given class */
        template<class T>
        QMap<QString, QFImporter*> createImporters(const QString& startswith=QString()) const {
            QMap<QString, QFImporter*>  result;
            for (int i=0; i<plugins.size(); i++) {
                QStringList ids=plugins[i]->getIDs();
                for (int j=0; j<ids.size(); j++) {
                    if (startswith.isEmpty() || ids[j].startsWith(startswith)) {
                        QFImporter* imp=plugins[i]->createImporter(ids[j]);
                        if (dynamic_cast<T>(imp)!=NULL) {
                            result[ids[j]]=imp;
                        } else {
                            delete imp;
                        }
                    }
                }


            }
            return result;
        }

        template<class T>
        QStringList getImporters(const QString& startswith=QString()) const {
            QStringList  result;
            for (int i=0; i<plugins.size(); i++) {
                QStringList ids=plugins[i]->getIDs();
                for (int j=0; j<ids.size(); j++) {
                    if (startswith.isEmpty() || ids[j].startsWith(startswith)) {
                        QFImporter* imp=plugins[i]->createImporter(ids[j]);
                        if (dynamic_cast<T>(imp)!=NULL) {
                            result.append(ids[j]);
                        }
                        delete imp;
                    }
                }


            }
            return result;
        }
        /** \brief return the plugin index for a given fit function id */
        int getPluginForID(QString id) const;
        /** \brief get plugins major version number */
        int getMajorVersion(int i);
        /** \brief get plugins minor version number */
        int getMinorVersion(int i);
        bool contains(const QString &ID);

        inline bool hasPluginForID(const QString& id) const {
            return getPluginForID(id)>=0;
        }

        void deinit();
        void init();

        void finalizePluginSearch();

        bool registerPlugin(const QString &filename_in, QObject *plugin, QFPluginHelpData &helpdata);

    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);

    private:
        QList<QFPluginImporters*> plugins;
        QStringList filenames;
        ProgramOptions* m_options;

};




#endif // QFIMPORTERMANAGER_H
