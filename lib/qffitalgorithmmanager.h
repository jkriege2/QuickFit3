/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#ifndef QFFITALGORITHMMANAGER_H
#define QFFITALGORITHMMANAGER_H

#include "lib_imexport.h"
#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QMap>
#include "programoptions.h"
#include "qfpluginservices.h"

class QFPluginFitAlgorithm; // forward
class QFFitAlgorithm;

/*! \brief this class manages the available fitting algorithms
    \ingroup qf3lib_fitting


*/
class QFLIB_EXPORT QFFitAlgorithmManager : public QObject {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFitAlgorithmManager(ProgramOptions* options, QObject* parent);
        /** Default destructor */
        virtual ~QFFitAlgorithmManager();

        /** \brief returns a list of the IDs of all available QFFitAlgorthm s. */
        QStringList getIDList() const;

        /** \brief returns a list of the IDs of all available QFFitAlgorthm s. */
        QStringList getIDList(int i) const;

        /** \brief return number of available plugins */
        int pluginCount() { return fitPlugins.size(); }

        /** \brief name for the plugin */
        virtual QString getName(int i) const;

        /** \brief plugin filename */
        virtual QString getPluginFilename(int i) const;

        /** \brief plugin filename */
        virtual QString getIconFilename(int i) const;

        /** \brief short description for the plugin */
        virtual QString getDescription(int i) const;

        /** \brief ID of the plugin */
        virtual QString getID(int i) const;

        /** \brief author the plugin */
        virtual QString getAuthor(int i) const;

        /** \brief copyright information the plugin */
        virtual QString getCopyright(int i) const;

        /** \brief weblink for the plugin */
        virtual QString getWeblink(int i) const;

        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList, QMap<QString, QFToolTipsData> &tooltips);

        /** \brief return the plugin index for a given fit function id */
        int getPluginForID(QString id) const;

        /** \brief get plugins major version number */
        int getMajorVersion(int i);

        /** \brief get plugins minor version number */
        int getMinorVersion(int i);

        /** \brief returns the plugins main help file (html) for a specified plugin \a ID. */
        QString getPluginHelp(int ID);
        /** \brief returns the plugins tutorial file (html) for a specified plugin \a ID. */
        QString getPluginTutorialMain(int ID);
        /** \brief returns the plugins tutorial file (html) for a specified plugin \a ID. */
        QString getPluginSettings(int ID);
        /** \brief returns the plugins copyright file (html) for a specified plugin \a ID. */
        QString getPluginCopyrightFile(int ID);
        /** \brief returns lists of titles (names) and links to all tutorials for for a specified plugin ID. */
        void getPluginTutorials(int ID, QStringList& names, QStringList& files);

        /** \brief returns a fit algorthms \a faID main help file (html) for a specified plugin \a ID. */
        QString getPluginHelp(int ID, QString faID);



        /** \brief create a new fit algorithm object instance
         *
         *  \note This function is thread-safe
         */
        QFFitAlgorithm* createAlgorithm(QString id, QObject* parent=NULL) const;

        /** \brief returns a pointer to the instance of the main manager object (singleton within a QuickFit3 instance) */
        static QFFitAlgorithmManager* getInstance() {
            if (QFPluginServices::getInstance()) {
                return QFPluginServices::getInstance()->getFitAlgorithmManager();
            }
            return NULL;
        }

        bool contains(const QString &ID);
    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);

    private:
        QList<QFPluginFitAlgorithm*> fitPlugins;
        QStringList filenames;
        ProgramOptions* m_options;
        QMutex* mutex;
    private:
};

#endif // QFFITALGORITHMMANAGER_H
