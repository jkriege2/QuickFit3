#ifndef QFPROJECTTREEMODEL_H
#define QFPROJECTTREEMODEL_H

#include <QAbstractItemModel>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include "lib_imexport.h"
#include "qfprojecttreemodelnode.h"

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
        QFProjectTreeModelNode* rootItem;
        QFProjectTreeModelNode* projectItem;
        QFProjectTreeModelNode* rdrFolderItem;
        QFProjectTreeModelNode* evalFolderItem;

        void createModelTree();
        /** \brief returns a QModelIndex which points to the specified record */
        QModelIndex index ( QFRawDataRecord* record, QFProjectTreeModelNode* folder) const;

        /** \brief returns a QModelIndex which points to the specified record */
        QModelIndex index ( QFEvaluationItem* record, QFProjectTreeModelNode* folder) const;


        QFEvaluationItem *getNextRecord(QFEvaluationItem *record, QFProjectTreeModelNode *folder, int t=0) const;
        QFRawDataRecord *getNextRecord(QFRawDataRecord *record, QFProjectTreeModelNode *folder, int t=0) const;

    public:
        virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
        virtual QModelIndex parent ( const QModelIndex & index ) const;
        virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

        QFProjectTreeModelNode* getTreeNodeByIndex(const QModelIndex& index) const;
        /** \brief return the raw data record specified by the given QModelIndex */
        QFRawDataRecord* getRawDataByIndex(const QModelIndex& index) const;

        /** \brief return the raw data record specified by the given QModelIndex */
        QFEvaluationItem* getEvaluationByIndex(const QModelIndex& index) const;

        /** \brief returns a QModelIndex which points to the specified record */
        QModelIndex index ( QFRawDataRecord* record) const;

        /** \brief returns the next record in the tree */
        QFRawDataRecord* getNextRecord(QFRawDataRecord* record) const;

        /** \brief returns the next record in the tree */
        QFEvaluationItem* getNextRecord(QFEvaluationItem* record) const;

        /** \brief returns a QModelIndex which points to the specified record */
        QModelIndex index ( QFEvaluationItem* record) const;

        QModelIndex index ( QFProjectTreeModelNode* node ) const;

        /** \brief returns the type of the supplied index */
        QFProjectTreeModelNode::nodeType classifyIndex(const QModelIndex& index) const;
    protected slots:
        /** \brief connected to the propertiesChanged() slot of the owning QFEvaluationItem. Used to
         *         tell the model that something has changed */
        void projectStructureChanged();

        void projectDataChanged();
};

#endif // QFPROJECTTREEMODEL_H
