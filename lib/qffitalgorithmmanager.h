#ifndef QFFITALGORITHMMANAGER_H
#define QFFITALGORITHMMANAGER_H

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QMap>
#include "qffitalgorithm.h"
#include "qfpluginfitalgorithm.h"

/*! \brief this class manages the available fitting algorithms
    \ingroup qf3lib_fitting


*/
class QFFitAlgorithmManager : public QObject {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFitAlgorithmManager(QObject* parent);
        /** Default destructor */
        virtual ~QFFitAlgorithmManager();

        /** \brief returns a list of the IDs of all available QFRawDataRecords. */
        QStringList getIDList() const;

        /** \brief returns a list of the IDs of all available QFRawDataRecords. */
        QStringList getIDList(int i) const;

        /** \brief return number of available plugins */
        int pluginCount() { return fitPlugins.size(); }

        /** \brief name for the plugin */
        virtual QString getName(int i) const;

        /** \brief plugin filename */
        virtual QString getFilename(int i) const;

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
        void searchPlugins(QString directory);

        /** \brief return the plugin index for a given fit function id */
        int getPluginForID(QString id) const;

        /** \brief get plugins major version number */
        int getMajorVersion(int i);

        /** \brief get plugins minor version number */
        int getMinorVersion(int i);

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
    private:
};

#endif // QFFITALGORITHMMANAGER_H
