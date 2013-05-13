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
        QString getPluginTutorial(int ID);
        /** \brief returns the plugins tutorial file (html) for a specified  ID. */
        QString getPluginSettings(int ID);
        /** \brief returns the plugins copyright file (html) for a specified  ID. */
        QString getPluginCopyrightFile(int ID);
        /** \brief returns a fit algorthms \a faID main help file (html) for a specified plugin \a ID. */
        QString getPluginHelp(int ID, QString faID);
        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList, QMap<QString, QFToolTipsData> &tooltips);

        QFImporter* createImporter(const QString& id) const;

        /** \brief return a list of the models which may be converted to the given class */
        template<class T>
        QMap<QString, QFImporter*> createImporters() {
            QMap<QString, QFImporter*>  result;
            for (int i=0; i<plugins.size(); i++) {
                QStringList ids=plugins[i]->getIDs();
                for (int j=0; j<ids.size(); j++) {
                    QFImporter* imp=plugins[i]->createImporter(ids[j]);
                    if (dynamic_cast<T>(imp)!=NULL) {
                        result[ids[j]]=imp;
                    } else {
                        delete imp;
                    }
                }


            }
            return result;
        }

        template<class T>
        QStringList getImporters() {
            QStringList  result;
            for (int i=0; i<plugins.size(); i++) {
                QStringList ids=plugins[i]->getIDs();
                for (int j=0; j<ids.size(); j++) {
                    QFImporter* imp=plugins[i]->createImporter(ids[j]);
                    if (dynamic_cast<T>(imp)!=NULL) {
                        result.append(ids[j]);
                    }
                    delete imp;
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
