#ifndef QFPROJECTTREEMODEL_H
#define QFPROJECTTREEMODEL_H

#include <QAbstractItemModel>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "lib_imexport.h"

// forward declaration
class QFProject;

/*! \brief model that allows Model/View access to the QuickFit project tree
    \ingroup qf3lib_project
*/
class QFLIB_EXPORT QFProjectTreeModel : public QAbstractItemModel {
        Q_OBJECT
    public:
        /** Default constructor */
        QFProjectTreeModel(QObject* parent);
        /** Default destructor */
        virtual ~QFProjectTreeModel();
        /** \brief initialize the model with a given project */
        void init(QFProject* p);
    protected:
        /** \brief the project represented by this data model */
        QFProject* current;
    public:
        virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
        virtual QModelIndex parent ( const QModelIndex & index ) const;
        virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        /** \brief return the raw data record specified by the given QModelIndex */
        QFRawDataRecord* getRawDataByIndex(const QModelIndex& index) const;

        /** \brief return the raw data record specified by the given QModelIndex */
        QFEvaluationItem* getEvaluationByIndex(const QModelIndex& index) const;

        /** \brief returns a QModelIndex which points to the specified record */
        QModelIndex index ( QFRawDataRecord* record, int column=0 ) const;

        /** \brief returns a QModelIndex which points to the specified record */
        QModelIndex index ( QFEvaluationItem* record, int column=0 ) const;

        enum nodeType {
            qfpntUnknown=0,
            qfpntRoot=1,
            qfpntProject=2,
            qfpntRawDataDir=3,
            qfpntRawDataRecord=4,
            qfpntEvaluationDir=5,
            qfpntEvaluationRecord=6
        };
        /** \brief returns the type of the supplied index */
        nodeType classifyIndex(const QModelIndex& index) const;
        /** \brief convert a nodeType into a string */
        QString nodeType2String( const nodeType nt) const;
    protected slots:
        /** \brief connected to the propertiesChanged() slot of the owning QFEvaluationItem. Used to
         *         tell the model that something has changed */
        void projectChanged(bool data=true);
};

#endif // QFPROJECTTREEMODEL_H
