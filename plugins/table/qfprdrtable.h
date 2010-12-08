#ifndef QFPRDRTABLE_H
#define QFPRDRTABLE_H

#include "qfpluginrawdata.h"


class QFPRDRTable : public QObject, public QFPluginRawDataRecordBase {
        Q_OBJECT
        Q_INTERFACES(QFPluginRawDataRecord)
    public:
        /** Default constructor */
        QFPRDRTable(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFPRDRTable();

        /** \brief Create a new QFRawDataRecord in the given project \a parent. Also adds the record to the project. */
        virtual QFRawDataRecord* createRecord(QFProject* parent);

        /*! \brief create menu items in \a menu that start insert of record

            The menu items should be connected to slots in the plugin.
        */
        virtual void registerToMenu(QMenu* menu);

        /** \brief short ID for the plugin */
        virtual QString getID() { return tr("table"); };

        /** \brief name for the plugin */
        virtual QString getName() { return tr("Data Table Plugin"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() { return tr("manages a table with arbitrary data in the cells"); };
    protected slots:
        /** \brief insert an editable table */
        void insertTable();

        /** \brief insert a file as read-only table */
        void insertTableFile();
    private:
};

#endif // QFPRDRTABLE_H
