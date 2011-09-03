#ifndef QFFITALGORITHMMANAGER_H
#define QFFITALGORITHMMANAGER_H

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
class QFFitAlgorithmManager : public QObject {
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
        void searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList);

        /** \brief return the plugin index for a given fit function id */
        int getPluginForID(QString id) const;

        /** \brief get plugins major version number */
        int getMajorVersion(int i);

        /** \brief get plugins minor version number */
        int getMinorVersion(int i);

        /** \brief returns the plugins main help file (html) for a specified plugin \a ID. */
        QString getPluginHelp(int ID);
        /** \brief returns the plugins tutorial file (html) for a specified plugin \a ID. */
        QString getPluginTutorial(int ID);
        /** \brief returns the plugins copyright file (html) for a specified plugin \a ID. */
        QString getPluginCopyrightFile(int ID);

        /** \brief returns a fit algorthms \a faID main help file (html) for a specified plugin \a ID. */
        QString getPluginHelp(int ID, QString faID);



        /** \brief create a new fit algorithm object instance */
        QFFitAlgorithm* createAlgorithm(QString id, QObject* parent=NULL) const;
    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);

    private:
        QList<QFPluginFitAlgorithm*> fitPlugins;
        QStringList filenames;
        ProgramOptions* m_options;
    private:
};

#endif // QFFITALGORITHMMANAGER_H
