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
#include "../lib/qt/qtlogfile.h"
#include "programoptions.h"
#include "qfevaluationitem.h"

/*! \brief this class is a factory class for QFEvaluationItem objects. You can
           request an object of a specific type by calling createItem() with
           the appropriate type string as argument

*/
class QFEvaluationItemFactory : public QObject {
        Q_OBJECT
    public:
        typedef QFEvaluationItem* (*QFEvaluationItemConstructorFunc)(QFProject* parent);

        /** \brief function type to call when starting a user-create operation (triggered by a QAction) */
        typedef void (*QFEvaluationItemUserCreateFunc)(QFProject* parent, ProgramOptions* options, QWidget* parentWidget, QtLogFile* logWidget, QStatusBar* statusbar, QProgressBar* progressBar);

        /*! \brief a function of this type registers several QActions in the given QMenu that allow the user
                  to create certain raw data records

            The QActions should have parent as their parent. Also add the actions to the menu, but you do NOT
            have to connect them to a specified slot. This is done automatically. Return all the QActions you
            created as a list that also conatins pointers to functions that should be called when the QAction
            is clicked.
        */
        typedef QList<QPair<QAction*, QFEvaluationItemUserCreateFunc> > (*QFEvaluationItemRegisterMenuFunc)(QWidget* parent, QMenu* menu);

        /** \brief this struct is used to internally manage the available QFRawDataRecord s */
        struct qfeiFactoryRecord {
            QString description;
            QString name;
            QFEvaluationItemConstructorFunc create;
            QFEvaluationItemRegisterMenuFunc registerMenu;
        };
    protected:
        /** \brief this map is used to internally manage the available QFRawDataRecord s.
         *         It maps from  the ID to the internal description.
         */
        QMap<QString, qfeiFactoryRecord> items;


    public:
        /** Default constructor */
        QFEvaluationItemFactory(QObject* parent=NULL);
        /** Default destructor */
        virtual ~QFEvaluationItemFactory();
        /** \brief returns a list of the IDs of all available QFEvaluationItem s. */
        inline QStringList getIDList() { return items.keys(); }

        /** \brief adds a new type of QFEvaluationItem
         *
         * \return \c true on success and \c false if an error occured (i.e. the given ID is already registeres
         */
        bool addRecord(QString ID, qfeiFactoryRecord desc);


        /** \brief returns the description for a specified QFEvaluationItem ID. */
        inline QString getDescription(QString ID) {
            if (items.contains(ID)) return items[ID].description;
            return QString("");
        };

        /** \brief returns the name for a specified QFEvaluationItem ID. */
        inline QString getName(QString ID) {
            if (items.contains(ID)) return items[ID].name;
            return QString("");
        };

        /** \brief returns a new object of the specified QFEvaluationItem ID. */
        inline QFEvaluationItem* createRecord(QString ID, QFProject* parent) {
            if (items.contains(ID)) return items[ID].create(parent);
            return NULL;
        };

        /** \brief register menu items for specified ID. */
        inline QList<QPair<QAction*, QFEvaluationItemUserCreateFunc> > registerMenu(QString ID, QWidget* parent, QMenu* menu)  {
            if (items.contains(ID)) {
                if (items[ID].registerMenu)
                    return items[ID].registerMenu(parent, menu);
            }
            return QList<QPair<QAction*, QFEvaluationItemUserCreateFunc> >();
        };
    private:
};


/** \brief this function returns a pointer to a global QFEvaluationItemFactory object */
QFEvaluationItemFactory* getEvaluationItemFactory();


#endif // QFEVALUATIONITEMFACTORY_H
