#ifndef QFPRDRTABLE_H
#define QFPRDRTABLE_H

#include "qfpluginrawdata.h"

/*!
    \defgroup qf3rdrdp_table Arbitrary Table Raw Data Record Plugin
    \ingroup qf3rawdataplugins
*/

/*! \brief table raw data record plugin class
    \ingroup qf3rdrdp_table
*/
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
        virtual QString getID() const  { return tr("table"); };

        /** \brief name for the plugin */
        virtual QString getName() const  { return tr("Data Table Plugin"); };

        /** \brief short description for the plugin */
        virtual QString getDescription() const  { return tr("manages a table with arbitrary data in the cells"); };

        /** \brief author the plugin */
        virtual QString getAuthor() const  { return tr("Jan Krieger"); };

        /** \brief copyright information the plugin */
        virtual QString getCopyright() const  { return tr("(c) 2010 by Jan Krieger"); };

        /** \brief weblink for the plugin */
        virtual QString getWeblink() const  { return tr(""); };

        /** \brief icon file for the plugin (from resource system or a separate file) */
        virtual QString getIconFilename() const  { return QString(":/table_insert.png"); };

    protected slots:
        /** \brief insert an editable table */
        void insertTable();

        /** \brief insert a file as read-only table */
        void insertTableFile();
    private:
};

#endif // QFPRDRTABLE_H
