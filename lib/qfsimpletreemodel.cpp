/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfsimpletreemodel.h"
#include <QStringList>
#include <QIcon>


QFSimpleTreeModel::QFSimpleTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    rootItem = new QFSimpleTreeModelItem("");
}


 QFSimpleTreeModel::~QFSimpleTreeModel()
 {
     delete rootItem;
 }

 int QFSimpleTreeModel::columnCount(const QModelIndex &parent) const
 {
     if (parent.isValid())
         return static_cast<QFSimpleTreeModelItem*>(parent.internalPointer())->columnCount();
     else
         return rootItem->columnCount();
 }

 QFSimpleTreeModelItem *QFSimpleTreeModel::itemForIndex(const QModelIndex &index) const
 {
     if (!index.isValid()) return NULL;
     return static_cast<QFSimpleTreeModelItem*>(index.internalPointer());
 }

 QFSimpleTreeModelItem *QFSimpleTreeModel::addFolderedItem(const QString &name_in, const QVariant &userData, QChar separator)
 {
     QStringList folders=name_in.split(separator);
     QString name=folders.value(folders.size()-1);
     if (folders.size()>0) folders.removeLast();
     return addFolderedItem(folders, name, userData);

 }

 QFSimpleTreeModelItem *QFSimpleTreeModel::addFolderedItem(const QString &folder, const QString &name, const QVariant &userData)
 {
     return addFolderedItem(QStringList(folder), name, userData);
 }

 QFSimpleTreeModelItem *QFSimpleTreeModel::addFolderedItem(const QStringList &folders_in, const QString &name, const QVariant &userData)
 {
     QStringList folders=folders_in;
     folders<<name;
     if (folders.size()<=0) return NULL;
     QFSimpleTreeModelItem* root=rootItem;

     for (int i=0; i<folders.size()-1; i++) {
         QFSimpleTreeModelItem* newroot=root;
         for (int j=0; j<root->childCount(); j++) {
             QFSimpleTreeModelItem* child=root->child(j);
             if (child && child->displayText()==folders[i] && child->isFolder()) {
                 newroot=child;
                 break;
             }
         }
         if (newroot==root) {
             newroot=new QFSimpleTreeModelItem(folders[i], root);
             newroot->setFolder(true);
             root->appendChild(newroot);
         }
         root=newroot;
     }
     QFSimpleTreeModelItem* item=new QFSimpleTreeModelItem(folders.last(), root);
     if (userData.isValid()) item->setData(userData);
     root->appendChild(item);
     return item;
 }

 void QFSimpleTreeModel::clear()
 {
     QFSimpleTreeModelItem *oldrootItem=rootItem;
     rootItem = new QFSimpleTreeModelItem("");
     delete oldrootItem;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    beginResetModel();
    endResetModel();
#else
    reset();
#endif
 }

 void QFSimpleTreeModel::sortTree()
 {
     beginResetModel();
     if (rootItem) rootItem->sort();
     endResetModel();
 }

 QVariant QFSimpleTreeModel::data(const QModelIndex &index, int role) const
 {
     if (!index.isValid())
         return QVariant();

     QFSimpleTreeModelItem *item = static_cast<QFSimpleTreeModelItem*>(index.internalPointer());

     if (!item) return QVariant();

     if (role==Qt::DecorationRole) {
         if (item->isFolder()) return QIcon(":/lib/projecttree_folder.png");
         else return item->data(role);
     }

     if (role==Qt::DisplayRole) return  item->displayText();

     return item->data(role);
 }

 Qt::ItemFlags QFSimpleTreeModel::flags(const QModelIndex &index) const
 {
     if (!index.isValid())
         return 0;

     QFSimpleTreeModelItem *item = static_cast<QFSimpleTreeModelItem*>(index.internalPointer());
     if (item && item->isFolder()) {
         return Qt::ItemIsEnabled;
     } else {
         return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
     }
 }

 QVariant QFSimpleTreeModel::headerData(int /*section*/, Qt::Orientation orientation, int role) const
 {
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
         return rootItem->data(role);

     return QVariant();
 }

 QModelIndex QFSimpleTreeModel::index(int row, int column, const QModelIndex &parent) const
 {
     if (!hasIndex(row, column, parent))
         return QModelIndex();

     QFSimpleTreeModelItem *parentItem;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<QFSimpleTreeModelItem*>(parent.internalPointer());

     QFSimpleTreeModelItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex QFSimpleTreeModel::index(const QFSimpleTreeModelItem *item) const
 {
     if (!item) return QModelIndex();
     QFSimpleTreeModelItem *parentItem=item->parent();
     if (!parentItem) parentItem=rootItem;

     int row=0;
     if (rootItem && item) row=parentItem->childRow(item);
     return createIndex(row, 0, (void*)item);
 }

 QModelIndex QFSimpleTreeModel::index(QVariant data, int role) const
 {
     if (!rootItem) return QModelIndex();
     return index(rootItem->findChild(data, role));
 }

 QModelIndex QFSimpleTreeModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     QFSimpleTreeModelItem *childItem = static_cast<QFSimpleTreeModelItem*>(index.internalPointer());
     QFSimpleTreeModelItem *parentItem = childItem->parent();

     if (parentItem == rootItem || !parentItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }

 int QFSimpleTreeModel::rowCount(const QModelIndex &parent) const
 {
     QFSimpleTreeModelItem *parentItem;
     if (parent.column() > 0)
         return 0;

     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<QFSimpleTreeModelItem*>(parent.internalPointer());

     return parentItem->childCount();
 }
























 QFSimpleTreeModelItem::QFSimpleTreeModelItem(const QString &text, QFSimpleTreeModelItem *parent)
 {
     parentItem = parent;
     userData[Qt::DisplayRole] = text;
     folder=false;
 }

 QFSimpleTreeModelItem::QFSimpleTreeModelItem(const QString &text, const QVariant &userRoleData, QFSimpleTreeModelItem *parent)
 {
     parentItem = parent;
     userData[Qt::DisplayRole] = text;
     userData[Qt::UserRole] = userRoleData;
     folder=false;
 }

 QFSimpleTreeModelItem::QFSimpleTreeModelItem(bool folder, const QString &text, QFSimpleTreeModelItem *parent)
 {
     parentItem = parent;
     userData[Qt::DisplayRole] = text;
     this->folder=folder;
 }

 QFSimpleTreeModelItem::QFSimpleTreeModelItem(bool folder, const QString &text, const QVariant &userRoleData, QFSimpleTreeModelItem *parent)
 {
     parentItem = parent;
     userData[Qt::DisplayRole] = text;
     userData[Qt::UserRole] = userRoleData;
     this->folder=folder;
 }

 QFSimpleTreeModelItem::QFSimpleTreeModelItem(QIcon icon, const QString &text, const QVariant &userRoleData, QFSimpleTreeModelItem *parent)
 {
     parentItem = parent;
     userData[Qt::DisplayRole] = text;
     userData[Qt::DecorationRole] = icon;
     userData[Qt::UserRole] = userRoleData;
     folder=false;
 }

 QFSimpleTreeModelItem::~QFSimpleTreeModelItem()
 {
     qDeleteAll(childItems);
 }

 void QFSimpleTreeModelItem::appendChild(QFSimpleTreeModelItem *item)
 {
     childItems.append(item);
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelItem::child(int row) const
 {
     return childItems.value(row);
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelItem::findChild(const QVariant &data, int role)
 {
     QFSimpleTreeModelItem * res=NULL;
     if (userData.value(role, QVariant())==data) return this;
     for (int i=0; i<childItems.size(); i++) {
         res=childItems[i]->findChild(data, role);
         if (res) return res;
     }
     return res;
 }

 int QFSimpleTreeModelItem::childRow(const QFSimpleTreeModelItem *child) const
 {
     for (int i=0; i<childItems.size(); i++) {
         if (childItems[i]==child) return i;
     }
     return -1;
 }

 int QFSimpleTreeModelItem::childCount() const
 {
     return childItems.count();
 }

 int QFSimpleTreeModelItem::columnCount() const
 {
     return 1;
 }

 QVariant QFSimpleTreeModelItem::displayText() const
 {
     return data(Qt::DisplayRole);
 }

 QVariant QFSimpleTreeModelItem::data(int role) const
 {
     return userData.value(role, QVariant());
 }

 QString QFSimpleTreeModelItem::text() const
 {
     return userData.value(Qt::DisplayRole, QVariant()).toString();
 }

 void QFSimpleTreeModelItem::setData(QVariant data, int role)
 {
     userData[role]=data;
 }

 void QFSimpleTreeModelItem::setIcon(const QIcon &icon)
 {
     setData(icon, Qt::DecorationRole);
 }

 void QFSimpleTreeModelItem::setForeground(QColor color)
 {
     setData(color, Qt::ForegroundRole);
     setData(color, Qt::TextColorRole);
 }

 void QFSimpleTreeModelItem::setBackground(QColor color)
 {
     setData(color, Qt::BackgroundColorRole);
 }

 void QFSimpleTreeModelItem::setText(const QString &text)
 {
     userData[Qt::DisplayRole] = text;
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelItem::parent() const
 {
     return parentItem;
 }

 bool QFSimpleTreeModelItem::isFolder() const
 {
     return this->folder;
 }

 void QFSimpleTreeModelItem::setFolder(bool folder)
 {
     this->folder=folder;
 }

 static bool QFSimpleTreeModelItem_lessthan(const QFSimpleTreeModelItem*s1, const QFSimpleTreeModelItem*s2)
 {
     if (s1 && s2) {
         if (s1->isFolder() && s2->isFolder()) return s1->text()<s2->text();
         if (!s1->isFolder() && s2->isFolder()) return false;
         if (s1->isFolder() && !s2->isFolder()) return true;
         if (!s1->isFolder() && !s2->isFolder()) return s1->text()<s2->text();
     }
     return false;
 }

 void QFSimpleTreeModelItem::sort()
 {
     qSort(childItems.begin(), childItems.end(), QFSimpleTreeModelItem_lessthan);
 }

 int QFSimpleTreeModelItem::row() const
 {
     if (parentItem)
         return parentItem->childItems.indexOf(const_cast<QFSimpleTreeModelItem*>(this));

     return 0;
 }


 QFSimpleTreeModelSortFilterProxyModel::QFSimpleTreeModelSortFilterProxyModel(QObject *parent):
     QSortFilterProxyModel(parent)
 {

 }

 bool QFSimpleTreeModelSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
 {
     QFSimpleTreeModel* tree=qobject_cast<QFSimpleTreeModel*>(sourceModel());
     if (tree) {
        QFSimpleTreeModelItem* item=tree->itemForIndex(tree->index(sourceRow, 0, sourceParent));
        if (item) {
            if (item->isFolder()) {
                bool acceptC=false;
                for (int i=0; i<item->childCount(); i++) {
                    acceptC=acceptC|filterAcceptsRow(i, tree->index(item));
                    if (acceptC) return true;
                }
                return false;
            } else {
                //QFSimpleTreeModelItem* parentItem=item->parent();
                return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
            }

        }
     } else {
         return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
     }
     return true;
 }


 QFSimpleTreeModelEnhancedComboBox::QFSimpleTreeModelEnhancedComboBox(QWidget *parent):
     QFEnhancedComboBox(parent)
 {
     m_model=new QFSimpleTreeModel(this);
     setModel(m_model);
 }

 QFSimpleTreeModel *QFSimpleTreeModelEnhancedComboBox::getModel() const
 {
     return m_model;
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelEnhancedComboBox::itemForIndex(const QModelIndex &index) const
 {
     return m_model->itemForIndex(index);
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelEnhancedComboBox::addFolderedItem(const QString &name, const QVariant &userData, QChar separator)
 {
     return m_model->addFolderedItem(name, userData, separator);
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelEnhancedComboBox::addFolderedItem(const QString &folder, const QString &name, const QVariant &userData)
 {
     return m_model->addFolderedItem(folder, name, userData);
 }

 QFSimpleTreeModelItem *QFSimpleTreeModelEnhancedComboBox::addFolderedItem(const QStringList &folders, const QString &name, const QVariant &userData)
 {
     return m_model->addFolderedItem(folders, name, userData);
 }
