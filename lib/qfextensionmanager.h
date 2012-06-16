#ifndef QFEXTENSIONMANAGER_H
#define QFEXTENSIONMANAGER_H

#include "lib_imexport.h"
#include <QObject>
#include <QMap>
#include <QWidget>
#include <QString>
#include "qfproject.h"
#include "programoptions.h"
#include "qfpluginservices.h"

// FORWARD DECLARATIONS
class QFExtension; // forward

/*! \brief manager class for QFExtension plugins
    \ingroup qf3extensionplugins
*/
class QFLIB_EXPORT QFExtensionManager : public QObject {
        Q_OBJECT
    public:
        /** \brief class constructor */
        QFExtensionManager(ProgramOptions* options, QObject* parent=NULL);
        /** \brief class destructor */
        virtual ~QFExtensionManager();


        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList);

        /** \brief distribute objects to plugins that allow interaction with the main application */
        void distribute(QFProject* project);

        /*! \brief initialize the plugins */
        void init(QFPluginServices* services, QWidget* parentWidget);

        /*! \brief deinitialize the plugins */
        void deinit();

        /*! \brief read settings in the plugins */
        void readPluginSettings(ProgramOptions* settings);

        /*! \brief write settings in the plugins */
        void writePluginSettings(ProgramOptions* settings);

        /** \brief returns a list of the IDs of all available QFExtensions. */
        inline QStringList getIDList() { return items.keys(); }

        /** \brief returns the description for a specified QFExtension ID. */
        QString getDescription(QString ID);

        /** \brief returns the name for a specified QFExtension ID. */
        QString getName(QString ID);


        /** \brief returns the author for a specified QFExtension ID. */
        QString getAuthor(QString ID);

        /** \brief returns the copyright for a specified QFExtension ID. */
        QString getCopyright(QString ID);
        /** \brief returns the weblink for a specified QFExtension ID. */
        QString getWeblink(QString ID);
        /** \brief returns the icon filename for a specified QFExtension ID. */
        QString getIconFilename(QString ID);
        /** \brief returns the plugins filename for a specified QFExtension ID. */
        QString getPluginFilename(QString ID);
        /** \brief get plugins major version number */
        int getMajorVersion(QString id);
        /** \brief get plugins minor version number */
        int getMinorVersion(QString id);

        /** \brief return a pointer to the given QFExtension instance */
        QFExtension* getInstance(QString id);
        /** \brief return a pointer to the given Object instance */
        QObject* getQObjectInstance(QString id);
        /** \brief return \c true if a plugin with the given ID is registered */
        bool contains(QString ID);

        /** \brief returns the plugins main help file (html) for a specified QFExtension ID. */
        QString getPluginHelp(QString ID);
        /** \brief returns the plugins tutorial file (html) for a specified QFExtension ID. */
        QString getPluginTutorial(QString ID);
        /** \brief returns the plugins copyright file (html) for a specified QFExtension ID. */
        QString getPluginCopyrightFile(QString ID);

        /** \brief returns a pointer to the instance of the main manager object (singleton within a QuickFit3 instance) */
        static QFExtensionManager* getInstance() {
            if (QFPluginServices::getInstance()) {
                return QFPluginServices::getInstance()->getExtensionManager();
            }
            return NULL;
        }

    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);
    protected:
        /** \brief this map is used to internally manage the available QFExtension s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QFExtension*> items;
        /** \brief this map is used to internally manage the available QFExtension s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QObject*> itemobjects;

        /** \brief this map contains the filenames of the plugin DLLs */
        QMap<QString, QString> filenames;

        ProgramOptions* m_options;

};

#endif // QFEXTENSIONMANAGER_H
