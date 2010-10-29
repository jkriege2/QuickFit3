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
#include "qt/qtlogfile.h"
#include "qfrawdatarecord.h"
#include "programoptions.h"


/*! \brief this class is a factory class for QFRawDataRecord objects. You can
           request an object of a specific type by calling createRecord() with
           the appropriate type string as argument

*/
class QFRawDataRecordFactory : public QObject {
        Q_OBJECT
    public:
        /** \brief function to call in order to create a new QFRawDataRecord object of a specific type */
        typedef QFRawDataRecord* (*QFRawDataRecordConstructorFunc)(QFProject* parent);

        /** \brief function type to call when starting a user-create operation (triggered by a QAction) */
        typedef void (*QFRawDataRecordUserCreateFunc)(QFProject* parent, ProgramOptions* options, QWidget* parentWidget, QtLogFile* logWidget, QStatusBar* statusbar, QProgressBar* progressBar);

        /*! \brief a function of this type registers several QActions in the given QMenu that allow the user
                  to create certain raw data records

            The QActions should have parent as their parent. Also add the actions to the menu, but you do NOT
            have to connect them to a specified slot. This is done automatically. Return all the QActions you
            created as a list that also conatins pointers to functions that should be called when the QAction
            is clicked.
        */
        typedef QList<QPair<QAction*, QFRawDataRecordUserCreateFunc> > (*QFRawDataRecordRegisterMenuFunc)(QWidget* parent, QMenu* menu);

        /** \brief this struct is used to internally manage the available QFRawDataRecord s */
        struct qfrdrFactoryRecord {
            QString description;
            QString name;
            QFRawDataRecordConstructorFunc create;
            QFRawDataRecordRegisterMenuFunc registerMenu;
        };
    protected:
        /** \brief this map is used to internally manage the available QFRawDataRecord s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, qfrdrFactoryRecord> items;

    public:
        /** Default constructor */
        QFRawDataRecordFactory(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFRawDataRecordFactory();

        /** \brief adds a new type of QFRawDataRecord
         *
         * \return \c true on success and \c false if an error occured (i.e. the given ID is already registeres
         */
        bool addRecord(QString ID, qfrdrFactoryRecord desc);

        /** \brief returns a list of the IDs of all available QFRawDataRecords. */
        inline QStringList getIDList() { return items.keys(); }

        /** \brief returns the description for a specified QFRawDataRecord ID. */
        inline QString getDescription(QString ID) {
            if (items.contains(ID)) return items[ID].description;
            return QString("");
        };

        /** \brief returns the name for a specified QFRawDataRecord ID. */
        inline QString getName(QString ID) {
            if (items.contains(ID)) return items[ID].name;
            return QString("");
        };

        /** \brief returns a new object (created by new) for a specified QFRawDataRecord ID. */
        inline QFRawDataRecord* createRecord(QString ID, QFProject* parent)  {
            if (items.contains(ID)) return items[ID].create(parent);
            return NULL;
        };

        /** \brief register menu items for specified ID. */
        inline QList<QPair<QAction*, QFRawDataRecordUserCreateFunc> > registerMenu(QString ID, QWidget* parent, QMenu* menu)  {
            if (items.contains(ID)) {
                if (items[ID].registerMenu)
                    return items[ID].registerMenu(parent, menu);
            }
            return QList<QPair<QAction*, QFRawDataRecordUserCreateFunc> >();
        };
    private:
};


/** \brief this function returns a pointer to a global QFRawDataRecordFactory object */
QFRawDataRecordFactory* getRawDataRecordFactory();

#endif // QFRAWDATARECORDFACTORY_H
