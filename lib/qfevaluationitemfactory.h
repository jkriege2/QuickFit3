#ifndef QFEVALUATIONITEMFACTORY_H
#define QFEVALUATIONITEMFACTORY_H

#include "lib_imexport.h"
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
#include "qfpluginservices.h"
#include "qfpluginevaluation.h"
#include "programoptions.h"

// forward
class QFEvaluationItem;

/*! \brief this class is a factory class for QFEvaluationItem objects. You can
           request an object of a specific type by calling createItem() with
           the appropriate type string as argument
    \ingroup qf3lib_project

*/
class QFLIB_EXPORT QFEvaluationItemFactory : public QObject {
        Q_OBJECT
    protected:
        /** \brief this map is used to internally manage the available QFEvaluationItem s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QFPluginEvaluationItem*> items;

        /** \brief this map contains the filenames of the plugin DLLs */
        QMap<QString, QString> filenames;

        ProgramOptions* m_options;

    public:
        /** Default constructor */
        QFEvaluationItemFactory(ProgramOptions* options, QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEvaluationItemFactory();


        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList);

        /** \brief distribute objects to plugins that allow interaction with the main application */
        void distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent);

        /** \brief deinitialize all plugins */
        void deinit();

        /** \brief returns a list of the IDs of all available QFEvaluationItems. */
        QStringList getIDList();

        /** \brief returns the description for a specified QFEvaluationItem ID. */
        QString getDescription(QString ID);

        /** \brief returns the name for a specified QFEvaluationItem ID. */
        QString getName(QString ID);

        /** \brief returns a new object (created by new) for a specified QFEvaluationItem ID. */
        QFEvaluationItem* createRecord(QString ID, QFPluginServices* services, QFProject* parent);


        /** \brief returns the author for a specified QFEvaluationItem ID. */
        QString getAuthor(QString ID);

        /** \brief returns the copyright for a specified QFEvaluationItem ID. */
        QString getCopyright(QString ID);
        /** \brief returns the weblink for a specified QFEvaluationItem ID. */
        QString getWeblink(QString ID);
        /** \brief returns the icon filename for a specified QFEvaluationItem ID. */
        QString getIconFilename(QString ID);
        /** \brief returns the plugins filename for a specified QFEvaluationItem ID. */
        QString getPluginFilename(QString ID);
        /** \brief returns the plugins main help file (html) for a specified QFEvaluationItem ID. */
        QString getPluginHelp(QString ID);
        /** \brief returns the plugins tutorial file (html) for a specified QFEvaluationItem ID. */
        QString getPluginTutorial(QString ID);
        /** \brief returns the plugins settings file (html) for a specified QFEvaluationItem ID. */
        QString getPluginSettings(QString ID);
        /** \brief returns the plugins copyright file (html) for a specified QFEvaluationItem ID. */
        QString getPluginCopyrightFile(QString ID);

        /** \brief return \c true if a plugin with the given ID is registered */
        bool contains(QString ID);

        /** \brief register menu items for specified ID. */
        void registerMenu(QString ID, QMenu* menu);

        /** \brief get plugins major version number */
        int getMajorVersion(QString id);
        /** \brief get plugins minor version number */
        int getMinorVersion(QString id);

    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);
};


/** \brief this function returns a pointer to a global QFEvaluationItemFactory object */
QFEvaluationItemFactory* getEvaluationItemFactory();


#endif // QFEVALUATIONITEMFACTORY_H
