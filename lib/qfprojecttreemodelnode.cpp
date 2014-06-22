/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#include "qfprojecttreemodelnode.h"
#include "qfproject.h"
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"

QFProjectTreeModelNode::QFProjectTreeModelNode(QFProjectTreeModelNode::nodeType type, QFProjectTreeModelNode *parent) {
    this->parentItem = parent;
    this->m_type=type;
    this->rdr=NULL;
    this->evalItem=NULL;
    this->m_title="";
    this->m_toolHelp="";
    this->m_project=NULL;
}

QFProjectTreeModelNode::~QFProjectTreeModelNode() {
    qDeleteAll(childItems);
}

QFProjectTreeModelNode::nodeType QFProjectTreeModelNode::type() const {
    return m_type;
}

QFProjectTreeModelNode* QFProjectTreeModelNode::addChild(QFProjectTreeModelNode::nodeType type, const QString& title) {
    QFProjectTreeModelNode* nn=new QFProjectTreeModelNode(type, this);
    nn->setTitle(title);
    appendChild(nn);
    return nn;
}

QFProjectTreeModelNode* QFProjectTreeModelNode::addChild(QFRawDataRecord* rdr) {
    QFProjectTreeModelNode* nn=new QFProjectTreeModelNode(QFProjectTreeModelNode::qfpntRawDataRecord, this);
    nn->setRawDataRecord(rdr);
    nn->setToolHelp(tr("a raw data record<br>"
                       "&nbsp;&nbsp;&bull;&nbsp; double-click to open the record in a new window"));
    appendChild(nn);
    return nn;
}

QFProjectTreeModelNode* QFProjectTreeModelNode::addChild(QFEvaluationItem* item) {
    QFProjectTreeModelNode* nn=new QFProjectTreeModelNode(QFProjectTreeModelNode::qfpntEvaluationRecord, this);
    nn->setEvaluationItem(item);
    nn->setToolHelp(tr("an evaluation item<br>"
                       "&nbsp;&nbsp;&bull;&nbsp; double-click to open the evaluation window"));
    appendChild(nn);
    return nn;
}

QFProjectTreeModelNode* QFProjectTreeModelNode::addChildFolder(const QString& title) {
    QStringList flds=title.split("/", QString::SkipEmptyParts);
    QString newfld=flds.value(0,"");
    QString otherflds="";
    if (flds.count()>1) {
        flds.removeAt(0);
        otherflds=flds.join("/");
    } else {
        flds.clear();
    }
    QFProjectTreeModelNode* nn=NULL;
    for (int i=0; i<childItems.count(); i++) {
        if (childItems[i]) {
            if (childItems[i]->type()==QFProjectTreeModelNode::qfpntDirectory && childItems[i]->title()==newfld) {
                nn=childItems[i];
                break;
            }
        }
    }
    if (!nn) {
        nn=new QFProjectTreeModelNode(QFProjectTreeModelNode::qfpntDirectory, this);
        nn->setTitle(newfld);
        nn->setProject(project());
        nn->setToolHelp(tr("a folder pooling several items in the tree together"));
        appendChild(nn);
    }
    if (!otherflds.isEmpty()) {
        return nn->addChildFolder(otherflds);
    } else {
        return nn;
    }
}

QFProject* QFProjectTreeModelNode::project() const {
    if (m_type==QFProjectTreeModelNode::qfpntRawDataRecord && rdr) {
        return rdr->getProject();
    } else if (m_type==QFProjectTreeModelNode::qfpntEvaluationRecord && evalItem) {
        return evalItem->getProject();
    }

    return m_project;
}

void QFProjectTreeModelNode::setProject(QFProject* project) {
    m_project=project;
}

QList<QFProjectTreeModelNode *> QFProjectTreeModelNode::getAllChildRawDataRecords()
{
    QList<QFProjectTreeModelNode *> list;
    if (m_type==QFProjectTreeModelNode::qfpntRawDataRecord) list<<this;
    for (int i=0; i<childItems.size(); i++) {
        if (childItems[i]) list.append(childItems[i]->getAllChildRawDataRecords());
    }
    list=removeQListDouplicates(list);
    return list;
}

QList<QFProjectTreeModelNode *> QFProjectTreeModelNode::getAllChildren()
{
    QList<QFProjectTreeModelNode *> list;
    list<<this;
    for (int i=0; i<childItems.size(); i++) {
        if (childItems[i]) list.append(childItems[i]->getAllChildRawDataRecords());
    }
    list=removeQListDouplicates(list);
    return list;
}

QList<QFProjectTreeModelNode *> QFProjectTreeModelNode::getAllChildrenRDRandEval()
{
    QList<QFProjectTreeModelNode *> list;
    if (m_type==QFProjectTreeModelNode::qfpntRawDataRecord) list<<this;
    if (m_type==QFProjectTreeModelNode::qfpntEvaluationRecord) list<<this;
    for (int i=0; i<childItems.size(); i++) {
        if (childItems[i]) list.append(childItems[i]->getAllChildRawDataRecords());
    }
    list=removeQListDouplicates(list);
    return list;
}

QString QFProjectTreeModelNode::getPath()
{
    QFProjectTreeModelNode* n=this;
    QString path="";
    if (type()==QFProjectTreeModelNode::qfpntDirectory) path=m_title;
    while (n->parent()) {
        n=n->parent();
        if (n->type()==QFProjectTreeModelNode::qfpntDirectory
            && n->parent() && n->parent()->type()!=QFProjectTreeModelNode::qfpntProject
                           && n->parent()->type()!=QFProjectTreeModelNode::qfpntRoot
                           && n->parent()->type()!=QFProjectTreeModelNode::qfpntUnknown) path=n->title()+"/"+path;
        if (n->type()==QFProjectTreeModelNode::qfpntProject) break;
        if (n->type()==QFProjectTreeModelNode::qfpntRoot) break;
        if (n->type()==QFProjectTreeModelNode::qfpntUnknown) break;
    }
    return path;
}


QIcon QFProjectTreeModelNode::icon() const {
    if (m_type==QFProjectTreeModelNode::qfpntProject) {
        return QIcon(":/lib/projecttree_project.png");
    } else if (m_type==QFProjectTreeModelNode::qfpntDirectory) {
        return QIcon(":/lib/projecttree_folderopen.png");
    } else if (m_type==QFProjectTreeModelNode::qfpntRawDataRecord) {
        if (rdr) return rdr->getSmallIcon();
        return QIcon(":/lib/projecttree_emptyrdr.png");
    } else if (m_type==QFProjectTreeModelNode::qfpntEvaluationRecord) {
        if (evalItem) return evalItem->getSmallIcon();
        return QIcon(":/lib/projecttree_emptyeval.png");
    }

    return QIcon();
}

void QFProjectTreeModelNode::setTitle(QString title) {
    this->m_title=title;
}

void QFProjectTreeModelNode::setToolHelp(QString title) {
    this->m_toolHelp=title;
}

void QFProjectTreeModelNode::setEvaluationItem(QFEvaluationItem* item) {
    this->evalItem=item;
}

void QFProjectTreeModelNode::setRawDataRecord(QFRawDataRecord* rdr) {
    this->rdr=rdr;
}

QString QFProjectTreeModelNode::title() const {
    if (m_type==QFProjectTreeModelNode::qfpntProject) {
        if (project()) return project()->getName();
        return tr("Project");
    } else if (m_type==QFProjectTreeModelNode::qfpntRawDataRecord) {
        if (rdr) return rdr->getName();
        return "";
    } else if (m_type==QFProjectTreeModelNode::qfpntEvaluationRecord) {
        if (evalItem) return evalItem->getName();
        return "";
    }
    return m_title;
}

QString QFProjectTreeModelNode::toolHelp() const {
    return m_toolHelp;
}

QFEvaluationItem* QFProjectTreeModelNode::evaluationItem() const {
    return evalItem;
}

QFRawDataRecord* QFProjectTreeModelNode::rawDataRecord() const {
    return rdr;
}

void QFProjectTreeModelNode::appendChild(QFProjectTreeModelNode *item) {
    childItems.append(item);
}

QFProjectTreeModelNode *QFProjectTreeModelNode::child(int row) {
    return childItems.value(row);
}

int QFProjectTreeModelNode::childCount() const {
    return childItems.count();
}



QFProjectTreeModelNode *QFProjectTreeModelNode::parent() {
    return parentItem;
}

int QFProjectTreeModelNode::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<QFProjectTreeModelNode*>(this));

    return 0;
}
