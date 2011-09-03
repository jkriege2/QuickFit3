#ifndef QFRAWDATARECORDFACTORY_H
#define QFRAWDATARECORDFACTORY_H

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
#include "qfpluginrawdata.h"
#include "qfpluginservices.h"
#include "lib_imexport.h"

class QFRawDataRecord;

/*! \brief this class is a factory class for QFRawDataRecord objects. You can
           request an object of a specific type by calling createRecord() with
           the appropriate type string as argument
    \ingroup qf3lib_project

*/
class QFLIB_EXPORT QFRawDataRecordFactory : public QObject {
        Q_OBJECT
    public:

    protected:
        /** \brief this map is used to internally manage the available QFRawDataRecord s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QFPluginRawDataRecord*> items;

        /** \brief this map contains the filenames of the plugin DLLs */
        QMap<QString, QString> filenames;

        ProgramOptions* m_options;

    public:
        /** Default constructor */
        QFRawDataRecordFactory(ProgramOptions* options, QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRawDataRecordFactory();

        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory, QList<QFPluginServices::HelpDirectoryInfo>* pluginHelpList);

        /** \brief distribute objects to plugins that allow interaction with the main application */
        void distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent);


        /** \brief returns a list of the IDs of all available QFRawDataRecords. */
        QStringList getIDList();

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

        /** \brief returns the plugins main help file (html) for a specified QFRawDataRecord ID. */
        QString getPluginHelp(QString ID);
        /** \brief returns the plugins tutorial file (html) for a specified QFRawDataRecord ID. */
        QString getPluginTutorial(QString ID);
        /** \brief returns the plugins copyright file (html) for a specified QFRawDataRecord ID. */
        QString getPluginCopyrightFile(QString ID);

        /** \brief get plugins major version number */
        int getMajorVersion(QString id);
        /** \brief get plugins minor version number */
        int getMinorVersion(QString id);



        /** \brief returns a new object (created by new) for a specified QFRawDataRecord ID. */
        QFRawDataRecord* createRecord(QString ID, QFProject* parent);

        /** \brief register menu items for specified ID. */
        void registerMenu(QString ID, QMenu* menu);
    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);
    private:
};



#endif // QFRAWDATARECORDFACTORY_H
