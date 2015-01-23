/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfprojecttreemodel.h"
#include "qfproject.h"
#include "qfevaluationitem.h"
QFProjectTreeModel::QFProjectTreeModel(QObject* parent):
    QAbstractItemModel(parent)
{
    current=NULL;
    rootItem=NULL;
    rdrFolderItem=NULL;
    evalFolderItem=NULL;
    projectItem=NULL;
    displayEval=displayRDR=true;
    groupCol=-1;
    roleCol=-1;
    groupBaseColor=QColor("aliceblue");
    displayGroupAsColor=true;
    rdrTypeFiler="";
    evalRDRApplicable=NULL;
}

QFProjectTreeModel::~QFProjectTreeModel()
{
    delete rootItem;
    rootItem=NULL;
    rdrFolderItem=NULL;
    evalFolderItem=NULL;
    projectItem=NULL;
}

void QFProjectTreeModel::init(QFProject* p, bool displayRDR, bool displayEval) {
    if (current) disconnect(current, SIGNAL(structureChanged(bool)), this , SLOT(projectStructureChanged()));
    if (current) disconnect(current, SIGNAL(propertiesChanged()), this , SLOT(projectDataChanged()));
    current=p;
    this->displayEval=displayEval;
    this->displayRDR=displayRDR;
    rdrUnchecked.clear();
    evaluationUnchecked.clear();
    createModelTree();
    if (current) {
        connect(current, SIGNAL(structureChanged()), this , SLOT(projectStructureChanged()));
        connect(current, SIGNAL(propertiesChanged()), this , SLOT(projectDataChanged()));
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

QSet<int> QFProjectTreeModel::getSelectedRDR() const
{
    QSet<int> res;
    if (current) {
        QList<int> ids=current->getRawDataIDList();
        for (int i=0; i<ids.size(); i++) {
            if (!rdrUnchecked.contains(ids[i])) res<<ids[i];
        }
    }
    return res;
}

QSet<int> QFProjectTreeModel::getSelectedEvaluations() const
{
    QSet<int> res;
    if (current) {
        QList<int> ids=current->getEvaluationIDList();
        for (int i=0; i<ids.size(); i++) {
            if (!evaluationUnchecked.contains(ids[i])) res<<ids[i];
        }
    }
    return res;

}

void QFProjectTreeModel::createModelTree() {
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    if (rootItem) {
        delete rootItem;
        rootItem=NULL;
        rdrFolderItem=NULL;
        evalFolderItem=NULL;
        projectItem=NULL;
    }

    QFProjectTreeModelNode* newRoot=new QFProjectTreeModelNode(QFProjectTreeModelNode::qfpntRoot, NULL);
    projectItem=newRoot->addChild(QFProjectTreeModelNode::qfpntProject, tr("Project"));
    projectItem->setProject(current);
    if (displayRDR) {
        rdrFolderItem=projectItem->addChildFolder(tr("Raw Data Records"));
        for (int i=0; i<current->getRawDataCount(); i++) {
            QFRawDataRecord* rec=current->getRawDataByNum(i);
            if (rec) {
                if (!evalRDRApplicable || (evalRDRApplicable && evalRDRApplicable->isApplicable(rec))) {
                    if (rdrTypeFiler.isEmpty() || (!rdrTypeFiler.isEmpty() && rec->getType().toLower()==rdrTypeFiler.toLower())) {
                        if (rec->getFolder().isEmpty()) rdrFolderItem->addChild(rec);
                        else {
                            QFProjectTreeModelNode* fld=rdrFolderItem->addChildFolder(rec->getFolder());
                            if (fld) fld->addChild(rec);
                            else rdrFolderItem->addChild(rec);
                        }
                    }
                }
            }
        }
    }
    if (displayEval) {
        evalFolderItem=projectItem->addChildFolder(tr("Evaluation Items"));
        for (int i=0; i<current->getEvaluationCount(); i++) {
            evalFolderItem->addChild(current->getEvaluationByNum(i));
        }
    }

    rootItem=newRoot;
    QApplication::restoreOverrideCursor();
}

int QFProjectTreeModel::columnCount ( const QModelIndex & parent ) const {
    if (!current || !rootItem) return 0;
    return qMax(qMax(1, groupCol+1), roleCol+1);
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
        case QFProjectTreeModelNode::qfpntProject:
            if (current && !current->isDummy()) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
            else return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        case QFProjectTreeModelNode::qfpntRawDataRecord:
            if (current && !current->isDummy()) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
            else return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        case QFProjectTreeModelNode::qfpntEvaluationRecord:
            if (current && !current->isDummy()) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
            else return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
        case QFProjectTreeModelNode::qfpntDirectory: {
                QFProjectTreeModelNode* node=getTreeNodeByIndex(index.parent());
                if (current && !current->isDummy() && node && node->type()!=QFProjectTreeModelNode::qfpntProject && node->type()!=QFProjectTreeModelNode::qfpntRoot) {
                    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
                }
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
            }
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

    int col=index.column();

    if (item) {
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            if (col==0) return item->title();
            if (col==groupCol && nt==QFProjectTreeModelNode::qfpntRawDataRecord && item->rawDataRecord()) return item->rawDataRecord()->getGroupName();
            if (col==roleCol && nt==QFProjectTreeModelNode::qfpntRawDataRecord && item->rawDataRecord()) return item->rawDataRecord()->getRole();
        } else if (role == Qt::DecorationRole) {
            if (col==0) return item->icon();
        } else if (role == Qt::ToolTipRole || role == Qt::StatusTipRole) {
            if (col==0) return item->toolHelp();
        } else if (role == Qt::CheckStateRole && current->isDummy()) {
            if (col==0 && nt==QFProjectTreeModelNode::qfpntEvaluationRecord) {
                if (evaluationUnchecked.contains(item->evaluationItem()->getID())) return Qt::Unchecked;
                else return Qt::Checked;
            }
            if (col==0 && nt==QFProjectTreeModelNode::qfpntRawDataRecord) {
                if (rdrUnchecked.contains(item->rawDataRecord()->getID())) return Qt::Unchecked;
                else return Qt::Checked;
            }
            if (col==0 && nt==QFProjectTreeModelNode::qfpntDirectory) {
                QFProjectTreeModelNode* dirNode=getTreeNodeByIndex(index);
                QList<QFProjectTreeModelNode*> l=dirNode->getAllChildRawDataRecords();
                QMap<QFRawDataRecord*, QString> m;
                Qt::CheckState s=Qt::PartiallyChecked;
                bool allUnchecked=true;
                bool someChecked=false;
                bool allChecked=true;
                for (int i=0; i<l.size(); i++) {
                    if (l[i] && l[i]->type()==QFProjectTreeModelNode::qfpntRawDataRecord) {
                        QFRawDataRecord* rec=l[i]->rawDataRecord();
                        if (rec){
                            if (!rdrUnchecked.contains(rec->getID())) {
                                allUnchecked=false;
                                someChecked=true;
                            } else {
                                allChecked=false;
                            }
                        }
                    }
                }
                if (allChecked) return Qt::Checked;
                if (allUnchecked) return  Qt::Unchecked;
                return Qt::PartiallyChecked;
            }
        } else if (displayGroupAsColor && role==Qt::BackgroundRole && nt==QFProjectTreeModelNode::qfpntRawDataRecord) {
            QFRawDataRecord* rec=getRawDataByIndex(index);
            if (rec) {
                double grpCount=qMax(10,current->getRDRGroupCount());
                double grp=rec->getGroup()+1;
                if (grp>0) {
                    int colFactor=100+grp/grpCount*80.0;
                    //qDebug()<<grp<<grpCount<<"  -> "<<colFactor;
                    return groupBaseColor.darker(colFactor);
                }
            }
        }
    }
    return QVariant();
}

bool QFProjectTreeModel::setData (const QModelIndex &index, const QVariant &value, int role) {

    //qDebug()<<"QFProjectTreeModel::setData("<<index<<value<<role<<")";
    if (!current || !rootItem) return false;
    int col=index.column();
    QFProjectTreeModelNode::nodeType nt=classifyIndex(index);
    if (col==0 && nt==QFProjectTreeModelNode::qfpntProject && role==Qt::EditRole) {
        current->setName(value.toString());
        emit dataChanged(index, index);
        return true;
    }
    if (nt==QFProjectTreeModelNode::qfpntRawDataRecord) {
        if (role==Qt::EditRole) {
            QFRawDataRecord* rec=getRawDataByIndex(index);
            if (rec) {
                if (col==0) {
                    rec->setName(value.toString());
                    emit dataChanged(index, index);
                    return true;
                }
            }
        } else if (col==0 && role==Qt::CheckStateRole) {
            QFRawDataRecord* rec=getRawDataByIndex(index);
            if (rec) {
                if (value.toBool()) {
                    rdrUnchecked.remove(rec->getID());
                } else {
                    rdrUnchecked.insert(rec->getID());
                }
                emit dataChanged(index, index);
                return true;
            }
        }
    }
    if (nt==QFProjectTreeModelNode::qfpntEvaluationRecord) {
        if (col==0 && role==Qt::EditRole) {
            QFEvaluationItem* rec=getEvaluationByIndex(index);
            if (rec) {
                rec->setName(value.toString());
                emit dataChanged(index, index);
                return true;
            }
        } else if (col==0 && role==Qt::CheckStateRole) {
            QFEvaluationItem* rec=getEvaluationByIndex(index);
            if (rec) {
                if (value.toBool()) {
                    evaluationUnchecked.remove(rec->getID());
                } else {
                    evaluationUnchecked.insert(rec->getID());
                }
                emit dataChanged(index, index);
                return true;
            }

        }
    }
    if (nt==QFProjectTreeModelNode::qfpntDirectory) {
        if (col==0 && role==Qt::EditRole) {
            QFProjectTreeModelNode* dirNode=getTreeNodeByIndex(index);
            dirNode->setTitle(value.toString());
            QList<QFProjectTreeModelNode*> l=dirNode->getAllChildRawDataRecords();
            QMap<QFRawDataRecord*, QString> m;
            for (int i=0; i<l.size(); i++) {
                if (l[i] && l[i]->type()==QFProjectTreeModelNode::qfpntRawDataRecord) {
                    QFRawDataRecord* rec=l[i]->rawDataRecord();
                    if (rec) m[rec]=l[i]->getPath();
                }
            }
            bool sigEn=current->areSignalsEnabled();
            current->setSignalsEnabled(false, false);
            QMapIterator<QFRawDataRecord*, QString> mIt(m);
            while (mIt.hasNext()) {
                mIt.next();
                mIt.key()->setFolder(mIt.value());
                emit dataChanged(this->index(mIt.key()), this->index(mIt.key()));
                //qDebug()<<mIt.key()->getName()<<": set folder: "<<mIt.value();
            }
            emit dataChanged(index, index);
            current->setSignalsEnabled(sigEn, true);
            return true;
        } else if (col==0 && role==Qt::CheckStateRole) {
            QFProjectTreeModelNode* dirNode=getTreeNodeByIndex(index);
            QList<QFProjectTreeModelNode*> l=dirNode->getAllChildRawDataRecords();
            for (int i=0; i<l.size(); i++) {
                if (l[i] && l[i]->type()==QFProjectTreeModelNode::qfpntRawDataRecord) {
                    QFRawDataRecord* rec=l[i]->rawDataRecord();
                    if (rec){
                        if(value.toBool()) rdrUnchecked.remove(rec->getID());
                        else rdrUnchecked.insert(rec->getID());
                    }
                }
                emit dataChanged(this->index(l[i]), this->index(l[i]));
            }
            emit dataChanged(index, index);
            QModelIndex idx=index;
            while (idx.parent().isValid()) {
                emit dataChanged(idx.parent(), idx.parent());
                idx=idx.parent();
            }
            return true;
        }
    }

    return false;
}

QVariant QFProjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Horizontal) {
        if (role==Qt::DisplayRole) {
            if (section==roleCol) return tr("role");
            if (section==groupCol) return tr("group");
        }
    }
    return QVariant();
}

QFProjectTreeModelNode *QFProjectTreeModel::getTreeNodeByIndex(const QModelIndex &index) const
{
    if (!current || !rootItem) return NULL;
    QFProjectTreeModelNode *item = static_cast<QFProjectTreeModelNode*>(index.internalPointer());
    return item;
}


QModelIndex QFProjectTreeModel::index ( int row, int column, const QModelIndex & parent) const {
    if (!current || !rootItem) return QModelIndex();
    if (!hasIndex(row, column, parent)) return QModelIndex();

    QFProjectTreeModelNode *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<QFProjectTreeModelNode*>(parent.internalPointer());

    if (!parentItem) return QModelIndex();
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
    if (!childItem) return QModelIndex();
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

void QFProjectTreeModel::setDisplayRole(bool enabled)
{
    if (!enabled) {
        roleCol=-1;
    } else {
        if (groupCol>=0) {
            roleCol=2;
        } else {
            roleCol=1;
        }
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void QFProjectTreeModel::setDisplayGroup(bool enabled)
{
    if (!enabled) {
        groupCol=-1;
        if (roleCol>=0) roleCol=1;
    } else {
        if (roleCol>=0) {
            groupCol=1;
            roleCol=2;
        } else {
            groupCol=1;
        }
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void QFProjectTreeModel::setDisplayGroupAsColor(bool enabled)
{
    displayGroupAsColor=enabled;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void QFProjectTreeModel::setRDRTypeFilter(const QString &type)
{
    rdrTypeFiler=type;
    createModelTree();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}

void QFProjectTreeModel::setRDRApplicableFilter(QFEvaluationItem *eval)
{
    evalRDRApplicable=eval;
    createModelTree();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
}



void QFProjectTreeModel::projectStructureChanged() {
    createModelTree();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
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
