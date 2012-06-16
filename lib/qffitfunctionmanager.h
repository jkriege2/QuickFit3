#ifndef QFFITFUNCTIONMANAGER_H
#define QFFITFUNCTIONMANAGER_H

#include "lib_imexport.h"
#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include "qffitfunction.h"
#include "qfpluginfitfunction.h"
#include "programoptions.h"
#include "qfpluginservices.h"

/*! \brief this class manages the available fitting functions
    \ingroup qf3lib_fitting

    As not ever model (fitting function) is applicable to any problem, this class provides
    means to sleect only a sub-group of parameters. They are selected by the start of their
    ID string, so getModels("fcs_") will return all models that start with "fcs_".
*/
class QFLIB_EXPORT QFFitFunctionManager : public QObject {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFitFunctionManager(ProgramOptions* options, QObject* parent);
        /** Default destructor */
        virtual ~QFFitFunctionManager();

        /** \brief return a list of all implemented IDs */
        QStringList getIDList(int i) const ;

        /** \brief return number of available plugins */
        int pluginCount() { return fitPlugins.size(); }

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
        /** \brief returns the plugins copyright file (html) for a specified  ID. */
        QString getPluginCopyrightFile(int ID);
        /** \brief returns a fit algorthms \a faID main help file (html) for a specified plugin \a ID. */
        QString getPluginHelp(int ID, QString faID);
        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList);

        /** \brief return a list of the models which start with the specified string
         *         all instance are newly created with the given parent in every call! */
        QMap<QString, QFFitFunction*> getModels(QString id_start=QString(""), QObject* parent=NULL);

        /** \brief return the plugin index for a given fit function id */
        int getPluginForID(QString id) const;
        /** \brief get plugins major version number */
        int getMajorVersion(int i);
        /** \brief get plugins minor version number */
        int getMinorVersion(int i);

        /** \brief returns a pointer to the instance of the main manager object (singleton within a QuickFit3 instance) */
        static QFFitFunctionManager* getInstance() {
            if (QFPluginServices::getInstance()) {
                return QFPluginServices::getInstance()->getFitFunctionManager();
            }
            return NULL;
        }

    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);

    private:
        QList<QFPluginFitFunction*> fitPlugins;
        QStringList filenames;
        ProgramOptions* m_options;
};

#endif // QFFITFUNCTIONMANAGER_H
