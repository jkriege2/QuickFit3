#ifndef QFFITALGORITHMMANAGER_H
#define QFFITALGORITHMMANAGER_H

#include <QObject>
#include <QtPlugin>
#include <QString>
#include <QStringList>
#include <QMap>
#include "qffitalgorithm.h"
#include "qfpluginfitalgorithm.h"

class QFFitAlgorithmManager : public QObject {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFitAlgorithmManager(QObject* parent);
        /** Default destructor */
        virtual ~QFFitAlgorithmManager();

        /** \brief returns a list of the IDs of all available QFRawDataRecords. */
        inline QStringList getIDList() { return fitPlugins.keys(); }

        /** \brief name for the plugin */
        virtual QString getName(QString i) const {
            if (!fitPlugins.contains(i)) return "";
            return fitPlugins[i]->getName();
        }

        /** \brief plugin filename */
        virtual QString getFilename(QString i) const {
            if (!filenames.contains(i)) return "";
            return filenames[i];
        }

        /** \brief short description for the plugin */
        virtual QString getDescription(QString i) const {
            if (!fitPlugins.contains(i)) return "";
            return fitPlugins[i]->getDescription();
        }

        /** \brief author the plugin */
        virtual QString getAuthor(QString i) const {
            if (!fitPlugins.contains(i)) return "";
            return fitPlugins[i]->getAuthor();
        }

        /** \brief copyright information the plugin */
        virtual QString getCopyright(QString i) const {
            if (!fitPlugins.contains(i)) return "";
            return fitPlugins[i]->getCopyright();
        }

        /** \brief weblink for the plugin */
        virtual QString getWeblink(QString i) const {
            if (!fitPlugins.contains(i)) return "";
            return fitPlugins[i]->getWeblink();
        }

        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory);

        /** \brief create a new fit algorithm object instance */
        QFFitAlgorithm* createAlgorithm(QString id, QObject* parent=NULL);

    private:
        QMap<QString, QFPluginFitAlgorithm*> fitPlugins;
        QMap<QString, QString> filenames;
    private:
};

#endif // QFFITALGORITHMMANAGER_H
