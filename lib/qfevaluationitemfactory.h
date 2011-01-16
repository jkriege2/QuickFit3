#ifndef QFEVALUATIONITEMFACTORY_H
#define QFEVALUATIONITEMFACTORY_H

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

// forward
class QFEvaluationItem;

/*! \brief this class is a factory class for QFEvaluationItem objects. You can
           request an object of a specific type by calling createItem() with
           the appropriate type string as argument
    \ingroup qf3lib_project

*/
class QFEvaluationItemFactory : public QObject {
        Q_OBJECT
    protected:
        /** \brief this map is used to internally manage the available QFRawDataRecord s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, QFPluginEvaluationItem*> items;

        /** \brief this map contains the filenames of the plugin DLLs */
        QMap<QString, QString> filenames;


    public:
        /** Default constructor */
        QFEvaluationItemFactory(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEvaluationItemFactory();


        /** \brief search for raw data record plugins in the given directory */
        void searchPlugins(QString directory);

        /** \brief distribute objects to plugins that allow interaction with the main application */
        void distribute(QFProject* project, ProgramOptions* settings, QFPluginServices* services, QWidget* parent);


        /** \brief returns a list of the IDs of all available QFRawDataRecords. */
        inline QStringList getIDList() { return items.keys(); }

        /** \brief returns the description for a specified QFRawDataRecord ID. */
        inline QString getDescription(QString ID) {
            if (items.contains(ID)) return items[ID]->getDescription();
            return QString("");
        };

        /** \brief returns the name for a specified QFRawDataRecord ID. */
        inline QString getName(QString ID) {
            if (items.contains(ID)) return items[ID]->getName();
            return QString("");
        };

        /** \brief returns a new object (created by new) for a specified QFRawDataRecord ID. */
        inline QFEvaluationItem* createRecord(QString ID, QFPluginServices* services, QFProject* parent)  {
            if (items.contains(ID)) return items[ID]->createRecord(parent);
            return NULL;
        };


        /** \brief returns the author for a specified QFRawDataRecord ID. */
        inline QString getAuthor(QString ID) {
            if (items.contains(ID)) return items[ID]->getAuthor();
            return QString("");
        };

        /** \brief returns the copyright for a specified QFRawDataRecord ID. */
        inline QString getCopyright(QString ID) {
            if (items.contains(ID)) return items[ID]->getCopyright();
            return QString("");
        };
        /** \brief returns the weblink for a specified QFRawDataRecord ID. */
        inline QString getWeblink(QString ID) {
            if (items.contains(ID)) return items[ID]->getWeblink();
            return QString("");
        };
        /** \brief returns the icon filename for a specified QFRawDataRecord ID. */
        inline QString getIconFilename(QString ID) {
            if (items.contains(ID)) return items[ID]->getIconFilename();
            return QString("");
        };
        /** \brief returns the plugins filename for a specified QFRawDataRecord ID. */
        inline QString getPluginFilename(QString ID) {
            if (items.contains(ID)) return filenames[ID];
            return QString("");
        };

        /** \brief register menu items for specified ID. */
        inline void registerMenu(QString ID, QMenu* menu)  {
            if (items.contains(ID)) {
                return items[ID]->registerToMenu(menu);
            }
        };
    signals:
        /** \brief short one-line message "loaded plugin XXX ...", emitted during searchPlugins() */
        void showMessage(const QString& message);
        /** \brief long message reporting the loaded plugins, emitted during searchPlugins() */
        void showLongMessage(const QString& message);
};


/** \brief this function returns a pointer to a global QFEvaluationItemFactory object */
QFEvaluationItemFactory* getEvaluationItemFactory();


#endif // QFEVALUATIONITEMFACTORY_H
