#include "qfprojecttreemodel.h"
#include "qfproject.h"

QFProjectTreeModel::QFProjectTreeModel(QObject* parent):
    QAbstractItemModel(parent)
{
    current=NULL;
    rootItem=NULL;
    rdrFolderItem=NULL;
    evalFolderItem=NULL;
    projectItem=NULL;
}

QFProjectTreeModel::~QFProjectTreeModel()
{
    delete rootItem;
    rootItem=NULL;
    rdrFolderItem=NULL;
    evalFolderItem=NULL;
    projectItem=NULL;
}

void QFProjectTreeModel::init(QFProject* p) {
    if (current) disconnect(current, SIGNAL(structureChanged(bool)), this , SLOT(projectStructureChanged()));
    if (current) disconnect(current, SIGNAL(propertiesChanged()), this , SLOT(projectDataChanged()));
    current=p;
    createModelTree();
    connect(current, SIGNAL(structureChanged()), this , SLOT(projectStructureChanged()));
    connect(current, SIGNAL(propertiesChanged()), this , SLOT(projectDataChanged()));
    reset();
}

void QFProjectTreeModel::createModelTree() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QFProjectTreeModelNode* newRoot=new QFProjectTreeModelNode(QFProjectTreeModelNode::qfpntRoot, NULL);
    projectItem=newRoot->addChild(QFProjectTreeModelNode::qfpntProject, tr("Project"));
    projectItem->setProject(current);
    rdrFolderItem=projectItem->addChildFolder(tr("Raw Data Records"));
    for (int i=0; i<current->getRawDataCount(); i++) {
        QFRawDataRecord* rec=current->getRawDataByNum(i);
        if (rec->getFolder().isEmpty()) rdrFolderItem->addChild(rec);
        else {
            QFProjectTreeModelNode* fld=rdrFolderItem->addChildFolder(rec->getFolder());
            if (fld) fld->addChild(rec);
            else rdrFolderItem->addChild(rec);
        }
    }
    evalFolderItem=projectItem->addChildFolder(tr("Evaluation Items"));
    for (int i=0; i<current->getEvaluationCount(); i++) {
        evalFolderItem->addChild(current->getEvaluationByNum(i));
    }
    if (rootItem) {
        delete rootItem;
        rootItem=NULL;
        rdrFolderItem=NULL;
        evalFolderItem=NULL;
        projectItem=NULL;
    }
    rootItem=newRoot;
    QApplication::restoreOverrideCursor();
}

int QFProjectTreeModel::columnCount ( const QModelIndex & parent ) const {
    if (!current || !rootItem) return 0;
    if (parent.isValid())
        return static_cast<QFProjectTreeModelNode*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

int QFProjectTreeModel:: rowCount ( const QModelIndex & parent) const {
    if (!current || !rootItem) return 0;

    QFProjectTreeModelNode *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<QFProjectTreeModelNode*>(parent.internalPointer());

    return parentItem->childCount();
}

Qt::ItemFlags QFProjectTreeModel::flags(const QModelIndex &index) const {
    if (!current || !rootItem) return 0;

    QFProjectTreeModelNode::nodeType nt=classifyIndex(index);
    switch(nt) {
        case QFProjectTreeModelNode::qfpntUnknown:
            return 0;
        case QFProjectTreeModelNode::qfpntRoot:
            return 0;
        default:
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    return 0;
 }

QVariant QFProjectTreeModel::data ( const QModelIndex & index, int role ) const {
    if (!current || !rootItem) return QVariant();

    if (!index.isValid())
        return QVariant();

    QFProjectTreeModelNode *item = static_cast<QFProjectTreeModelNode*>(index.internalPointer());
    QFProjectTreeModelNode::nodeType nt=classifyIndex(index);

    if (item) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return item->title();
        } else if (role == Qt::DecorationRole) {
            return item->icon();
        } else if (role == Qt::ToolTipRole || role == Qt::StatusTipRole) {
            return item->toolHelp();
        }
    }
    return QVariant();
}

bool QFProjectTreeModel::setData (const QModelIndex &index, const QVariant &value, int role) {
    return false;
}


QModelIndex QFProjectTreeModel::index ( int row, int column, const QModelIndex & parent) const {
    if (!current || !rootItem) return QModelIndex();
    if (!hasIndex(row, column, parent)) return QModelIndex();

    QFProjectTreeModelNode *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<QFProjectTreeModelNode*>(parent.internalPointer());

    QFProjectTreeModelNode *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QFProjectTreeModel::parent ( const QModelIndex & index ) const {
    if (!index.isValid())
        return QModelIndex();

    QFProjectTreeModelNode *childItem = static_cast<QFProjectTreeModelNode*>(index.internalPointer());
    QFProjectTreeModelNode *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

QFRawDataRecord* QFProjectTreeModel::getRawDataByIndex(const QModelIndex& index) const {
    if (!current || !rootItem) return NULL;
    QFProjectTreeModelNode *item = static_cast<QFProjectTreeModelNode*>(index.internalPointer());
    if (item) return item->rawDataRecord();
    return NULL;
}

QFEvaluationItem* QFProjectTreeModel::getEvaluationByIndex(const QModelIndex& index) const {
    if (!current || !rootItem) return NULL;
   QFProjectTreeModelNode *item = static_cast<QFProjectTreeModelNode*>(index.internalPointer());
   if (item) return item->evaluationItem();
   return NULL;
}


QModelIndex QFProjectTreeModel::index ( QFRawDataRecord* record, QFProjectTreeModelNode* folder) const {
    QModelIndex idx;
    if (folder) {
        if (folder->rawDataRecord()==record) {
            //qDebug()<<"found record "<<record<<"   folder->row()="<<folder->row();
            return createIndex(folder->row(), 0, folder);
        }
        for (int i=0; i<folder->childCount(); i++) {
            idx=index(record, folder->child(i));
            if (idx.isValid()) return idx;
        }
    }
    return idx;
}

QModelIndex QFProjectTreeModel::index ( QFEvaluationItem* record, QFProjectTreeModelNode* folder) const {
    QModelIndex idx;
    if (folder) {
        if (folder->evaluationItem()==record) return createIndex(folder->row(), 0, folder);
        for (int i=0; i<folder->childCount(); i++) {
            idx=index(record, folder->child(i));
            if (idx.isValid()) break;
        }
    }
    return idx;
}

QModelIndex QFProjectTreeModel::index ( QFRawDataRecord* record ) const {
    //qDebug()<<"index("<<record<<"):   current="<<current;
    if (!current || !record ) return QModelIndex();
    //qDebug()<<"index("<<record<<"):   current->rawDataExists(record)="<<current->rawDataExists(record);
    if (!current->rawDataExists(record)) return QModelIndex();
    //qDebug()<<"index("<<record<<"):   index="<<index(record, rdrFolderItem);
    return index(record, rdrFolderItem);
}

QModelIndex QFProjectTreeModel::index ( QFEvaluationItem* record ) const {
    if (!current || !record) return QModelIndex();
    if (!current->evaluationExists(record)) return QModelIndex();
    return index(record, evalFolderItem);
}

QModelIndex QFProjectTreeModel::index ( QFProjectTreeModelNode* node ) const {
    if (node) {
        return createIndex(node->row(), 0, node);
    }
    return QModelIndex();
}

QFRawDataRecord* QFProjectTreeModel::getNextRecord ( QFRawDataRecord* record, QFProjectTreeModelNode* folder, int t) const {
    //qDebug()<<QString(t*2, ' ')<<"getNextRecord(RDR "<<record<<", "<<folder<<")";
   QFRawDataRecord* last=NULL;
    if (folder) {
        for (int i=folder->childCount()-1; i>=0; i--) {
            //qDebug()<<QString(t*2, ' ')<<"   getNextRecord(EV "<<record<<", "<<folder<<"):   i="<<i+1<<"/"<<folder->childCount()<<"  last="<<last;
            if (folder->child(i)->rawDataRecord()) {
                //qDebug()<<QString(t*2, ' ')<<"getNextRecord(RDR "<<record<<", "<<folder<<"):   i="<<i+1<<"/"<<folder->childCount();
                if (folder->child(i)->rawDataRecord()==record) {
                    //qDebug()<<QString(t*2, ' ')<<"getNextRecord(EV "<<record<<", "<<folder<<"):   i="<<i+1<<"/"<<folder->childCount()<<"  returning last="<<last;
                    return last;
                }
                last=folder->child(i)->rawDataRecord();
            } else {
                QFRawDataRecord* idx=getNextRecord(record, folder->child(i), t+1);
                //qDebug()<<QString(t*2, ' ')<<"getNextRecord(RDR "<<record<<", "<<folder<<"):   i="<<i+1<<"/"<<folder->childCount()<<"  returning idx?="<<idx;
                if (idx) return idx;
            }

        }
    }
    return NULL;
}

QFEvaluationItem* QFProjectTreeModel::getNextRecord ( QFEvaluationItem* record, QFProjectTreeModelNode* folder, int t) const {
    //qDebug()<<QString(t*2, ' ')<<"getNextRecord(EV "<<record<<", "<<folder<<")";
    QFEvaluationItem* last=NULL;
     if (folder) {
         for (int i=folder->childCount()-1; i>=0; i--) {
             //qDebug()<<QString(t*2, ' ')<<"getNextRecord(EV "<<record<<", "<<folder<<"):   i="<<i+1<<"/"<<folder->childCount()<<"  last="<<last;
             if (folder->child(i)->evaluationItem()) {
                 if (folder->child(i)->evaluationItem()==record) {
                     //qDebug()<<QString(t*2, ' ')<<"getNextRecord(EV "<<record<<", "<<folder<<"):   i="<<i+1<<"/"<<folder->childCount()<<"  returning last="<<last;
                     return last;
                 }
                 last=folder->child(i)->evaluationItem();
             } else {
                 QFEvaluationItem* idx=getNextRecord(record, folder->child(i), t+1);
                 //qDebug()<<QString(t*2, ' ')<<"getNextRecord(EV "<<record<<", "<<folder<<"):   i="<<i+1<<"/"<<folder->childCount()<<"  returning idx?="<<idx;
                 if (idx) return idx;
             }

         }
     }
     return NULL;
}

QFRawDataRecord *QFProjectTreeModel::getNextRecord(QFRawDataRecord *record) const {
    //qDebug()<<"getNextRecord(RDR "<<record<<")";
    if (!record) return NULL;
    return getNextRecord(record, rootItem);
}

QFEvaluationItem *QFProjectTreeModel::getNextRecord(QFEvaluationItem *record) const {
    //qDebug()<<"getNextRecord(EV "<<record<<")";
    if (!record) return NULL;
    return getNextRecord(record, rootItem);
}



QFProjectTreeModelNode::nodeType QFProjectTreeModel::classifyIndex(const QModelIndex& index) const {
    if (!index.isValid())
        return QFProjectTreeModelNode::qfpntUnknown;

    QFProjectTreeModelNode *item = static_cast<QFProjectTreeModelNode*>(index.internalPointer());

    if (item) return item->type();

    return QFProjectTreeModelNode::qfpntUnknown;
}



void QFProjectTreeModel::projectStructureChanged() {
    createModelTree();
    reset();
}


void QFProjectTreeModel::projectDataChanged() {
    if (projectItem) {
        QModelIndex idx=index(projectItem);
        emit dataChanged(idx, idx);
    }
    for (int i=0; i<current->getRawDataCount(); i++) {
        QFRawDataRecord* rec=current->getRawDataByNum(i);
        QModelIndex idx=index(rec);
        if (rec) {
            if (rec->getName()!=idx.data().toString()) emit dataChanged(idx, idx);
        }
    }
    if (projectItem) {
        evalFolderItem=projectItem->addChildFolder(tr("Evaluation Items"));
        for (int i=0; i<current->getEvaluationCount(); i++) {
            QFEvaluationItem* item=current->getEvaluationByNum(i);
            QModelIndex idx=index(item);
            if (item) {
                if (item->getName()!=idx.data().toString()) emit dataChanged(idx, idx);
            }
        }
    }
}
