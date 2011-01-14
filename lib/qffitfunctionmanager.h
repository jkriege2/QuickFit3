#ifndef QFFITFUNCTIONMANAGER_H
#define QFFITFUNCTIONMANAGER_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include "qffitfunction.h"
#include "qfpluginfitfunction.h"

/*! \brief this class manages the available fitting functions
    \ingroup qf3lib_fitting

    As not ever model (fitting function) is applicable to any problem, this class provides
    means to sleect only a sub-group of parameters. They are selected by the start of their
    ID string, so getModels("fcs_") will return all models that start with "fcs_".
*/
class QFFitFunctionManager : public QObject {
        Q_OBJECT
    public:
        /** Default constructor */
        QFFitFunctionManager(QObject* parent);
        /** Default destructor */
        virtual ~QFFitFunctionManager();

        /** \brief return a list of all implemented IDs */
        QStringList getIDList(int i) const ;

        /** \brief return number of available plugins */
        int pluginCount() { return fitPlugins.size(); }

        /** \brief name for the plugin */
        virtual QString getName(int i) const;

        /** \brief plugin filename */
        virtual QString getFilename(int i) const;

        /** \brief short description for the plugin */
        virtual QString getDescription(int i) const;

        /** \brief author the plugin */
        virtual QString getAuthor(int i) const;

        /** \brief copyright information the plugin */
        virtual QString getCopyright(int i) const;

        /** \brief weblink for the plugin */
        virtual QString getWeblink(int i) const;

        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory);

        /** \brief return a list of the models which start with the specified string
         *         all instance are newly created with the given parent in every call! */
        QMap<QString, QFFitFunction*> getModels(QString id_start=QString(""), QObject* parent=NULL);

        /** \brief return the plugin index for a given fit function id */
        int getPluginForID(QString id) const;

    private:
        QList<QFPluginFitFunction*> fitPlugins;
        QStringList filenames;
};

#endif // QFFITFUNCTIONMANAGER_H
