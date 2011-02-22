#ifndef QFEXTENSIONMANAGER_H
#define QFEXTENSIONMANAGER_H

#include <QObject>
#include <QMap>
#include <QWidget>
#include <QString>
#include "qfproject.h"
#include "programoptions.h"

// FORWARD DECLARATIONS
class QFExtension; // forward
class QFExtensionServices;

/*! \brief manager class for QFExtension plugins
    \ingroup qf3extensionplugins
*/
class QFExtensionManager : public QObject {
        Q_OBJECT
    public:
        /** \brief class constructor */
        QFExtensionManager(QObject* parent=NULL);
        /** \brief class destructor */
        virtual ~QFExtensionManager();


        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory);

        /** \brief distribute objects to plugins that allow interaction with the main application */
        void distribute(QFProject* project);

        /*! \brief initialize the plugins */
        void init(QFExtensionServices* services, QWidget* parentWidget);

        /*! \brief deinitialize the plugins */
        void deinit();

        /*! \brief read settings in the plugins */
        void readPluginSettings(ProgramOptions* settings);

        /*! \brief write settings in the plugins */
        void writePluginSettings(ProgramOptions* settings);

        /** \brief returns a list of the IDs of all available QFRawDataRecords. */
        inline QStringList getIDList() { return items.keys(); }

        /** \brief returns the description for a specified QFRawDataRecord ID. */
        QString getDescription(QString ID);

        /** \brief returns the name for a specified QFRawDataRecord ID. */
        QString getName(QString ID);


        /** \brief returns the author for a specified QFRawDataRecord ID. */
        QString getAuthor(QString ID);

        /** \brief returns the copyright for a specified QFRawDataRecord ID. */
        QString getCopyright(QString ID);
        /** \brief returns the weblink for a specified QFRawDataRecord ID. */
        QString getWeblink(QString ID);
        /** \brief returns the icon filename for a specified QFRawDataRecord ID. */
        QString getIconFilename(QString ID);
        /** \brief returns the plugins filename for a specified QFRawDataRecord ID. */
        QString getPluginFilename(QString ID);

        /** \brief return a pointer to the given QFExtension instance */
        QFExtension* getInstance(QString id);
        /** \brief return a pointer to the given Object instance */
        QObject* getQObjectInstance(QString id);


    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);
    protected:
        /** \brief this map is used to internally manage the available QFRawDataRecord s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QFExtension*> items;
        /** \brief this map is used to internally manage the available QFRawDataRecord s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QObject*> itemobjects;

        /** \brief this map contains the filenames of the plugin DLLs */
        QMap<QString, QString> filenames;

};

#endif // QFEXTENSIONMANAGER_H
