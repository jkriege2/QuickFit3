/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-07 20:51:17 +0100 (Sa, 07 Feb 2015) $  (revision $Rev: 3802 $)

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

#include "qfpseudotreemodel.h"
#include <QPainter>
#include <QPalette>
#include <QApplication>

QFPseudoTreeModel::QFPseudoTreeModel(QObject* parent):
    QAbstractListModel(parent)
{

}

QFPseudoTreeModel::~QFPseudoTreeModel()
{
    clear();
}

QVariant QFPseudoTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (isFolder(index)) {
        if (role==Qt::DecorationRole) {
            return QIcon(":/lib/projecttree_folder.png");
        } else if (role==Qt::DisplayRole) {
            int idx=folderIndexForIndex(index);
            if (idx>=0 && idx<m_folders.size()) return m_folders.value(idx);
        }
        return QVariant();
    }

    if (index.internalPointer()) {
        QFPseudoTreeModelItem *item = static_cast<QFPseudoTreeModelItem*>(index.internalPointer());

        if (!item) return QVariant();

        if (role==Qt::DisplayRole) return  item->displayText();
        if (role==Qt::DecorationRole) {
            QVariant d=item->data(role);
            int w=16;
            if (d.type()==QVariant::Icon) {
                w=32;
            }
            QPixmap ic(QSize(w,16));

            ic.fill(Qt::transparent);
            {
                QPainter p(&ic);
                QPen pen(QApplication::palette("QWidget").color(QPalette::Mid));
                pen.setStyle(Qt::DotLine);
                p.setPen(pen);
                if (isLastInFolder(index)) {
                    p.drawLine(8,0,8,8);
                } else {
                    p.drawLine(8,0,8,17);
                }
                p.drawLine(8,8,15,8);

                if (d.type()==QVariant::Icon) {
                    p.drawPixmap(16,0,16,16, d.value<QIcon>().pixmap(QSize(16,16)));
                }
            }
            return  ic;
        }

        return item->data(role);
    }
    return QVariant();
}

Qt::ItemFlags QFPseudoTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    if (isFolder(index)) {
        return Qt::ItemIsEnabled;
    }


    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant QFPseudoTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

QModelIndex QFPseudoTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    QFPseudoTreeModelItem *childItem = NULL;
    int r=0;
    for (int i=0; i<m_folders.size(); i++) {
        if (row==r) {
            return createIndex(row, 0, (void*)NULL);
            break;
        }
        r++;
        if (m_items.contains(m_folders[i])) {
            for (int j=0; j<m_items[m_folders[i]].size(); j++) {
                if (row==r) {
                    childItem=m_items[m_folders[i]].value(j, NULL);
                    return createIndex(row, 0, childItem);
                    break;
                }
                r++;
            }
        }
    }
    return QModelIndex();
}

QModelIndex QFPseudoTreeModel::index(const QFPseudoTreeModelItem *item) const
{
    if (!item) return QModelIndex();
    int r=0, row=-1;
    QFPseudoTreeModelItem *childItem = NULL;
    for (int i=0; i<m_folders.size(); i++) {
        r++;
        if (m_items.contains(m_folders[i])) {
            for (int j=0; j<m_items[m_folders[i]].size(); j++) {
                if (m_items[m_folders[i]].value(j, NULL)==item) {
                    row=r;
                    childItem=m_items[m_folders[i]].value(j, NULL);
                    break;
                }
                r++;
            }
        }
        if (row>=0) break;
    }
    if (row>=0) {
        return createIndex(row, 0, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex QFPseudoTreeModel::index(QVariant data, int role) const
{
    if (!data.isValid() || role<0) return QModelIndex();
    int r=0, row=-1;
    QFPseudoTreeModelItem *childItem = NULL;
    for (int i=0; i<m_folders.size(); i++) {
        if (m_items.contains(m_folders[i])) {
            r++;
            for (int j=0; j<m_items[m_folders[i]].size(); j++) {
                if (m_items[m_folders[i]].value(j, NULL) && m_items[m_folders[i]].value(j, NULL)->data(role)==data) {
                    row=r;
                    childItem=m_items[m_folders[i]].value(j, NULL);
                    break;
                }
                r++;
            }
        }
        if (row>=0) break;
    }
    if (row>=0) {
        return createIndex(row, 0, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex QFPseudoTreeModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int QFPseudoTreeModel::rowCount(const QModelIndex &parent) const
{
    int rows=m_folders.size();
    QMapIterator<QString, QList<QFPseudoTreeModelItem*> > it(m_items);
    while (it.hasNext()) {
        it.next();
        rows+=it.value().size();
    }
    return rows;
}

int QFPseudoTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QFPseudoTreeModelItem *QFPseudoTreeModel::itemForIndex(const QModelIndex &index) const
{
    if (!index.isValid()) return NULL;
    if (index.internalPointer()>=0) return static_cast<QFPseudoTreeModelItem*>(index.internalPointer());
    return NULL;
}

int QFPseudoTreeModel::folderIndexForIndex(const QModelIndex &index) const
{
    if (!index.isValid()) return -1;
    int r=0;
    for (int i=0; i<m_folders.size(); i++) {
        if (m_items.contains(m_folders[i])) {
            if (r==index.row()) return i;
            r++;
            for (int j=0; j<m_items[m_folders[i]].size(); j++) {
                if (r==index.row()) return i;
                r++;
            }
        }
    }
    return -1;
}

bool QFPseudoTreeModel::isFolder(const QModelIndex &index) const
{
    return index.isValid() && index.internalId()==0; // && (-index.internalId())-1<m_folders.size();
}

QList<int> QFPseudoTreeModel::folderContentsRows(int folder) const
{
    if (folder>=0 && folder<m_folders.size()) {
        QList<int> idx;
        for (int i=0; i<m_items[m_folders[folder]].size(); i++) {
            QModelIndex id=index(m_items[m_folders[folder]].value(i, NULL));
            if (id.isValid()) idx.append(id.row());
        }
        return idx;
    }
    return QList<int>();
}

QList<int> QFPseudoTreeModel::folderContentsRows(const QModelIndex &folder) const
{
    if (isFolder(folder)) {
        return folderContentsRows(folderIndexForIndex(folder));
    }
    return QList<int>();
}

bool QFPseudoTreeModel::isLastInFolder(const QModelIndex &index) const
{
    if (!index.isValid()) return true;
    QFPseudoTreeModelItem *childItem = itemForIndex(index);
    if (!childItem) return true;
    for (int i=0; i<m_folders.size(); i++) {
        if (m_items.contains(m_folders[i])) {
            for (int j=0; j<m_items[m_folders[i]].size(); j++) {
                if (m_items[m_folders[i]].value(j, NULL)==childItem) {
                    return j==m_items[m_folders[i]].size()-1;
                }
            }
        }
    }
    return true;
}

QFPseudoTreeModelItem *QFPseudoTreeModel::addFolderedItem(const QString &name_in, const QVariant &userData, QChar separator)
{
    QStringList folders=name_in.split(separator);
    QString name=folders.value(folders.size()-1);
    if (folders.size()>0) folders.removeLast();
    return addFolderedItem(folders.join("/"), name, userData);
}

QFPseudoTreeModelItem *QFPseudoTreeModel::addFolderedItem(const QString &folder, const QString &name, const QVariant &userData)
{
    if (!m_folders.contains(folder)) m_folders.append(folder);
    if (!m_items.contains(folder)) {
        m_items.insert(folder, QList<QFPseudoTreeModelItem*>());
    }
    QFPseudoTreeModelItem* item=new QFPseudoTreeModelItem(name);
    if (userData.isValid()) item->setData(userData);
    m_items[folder].append(item);
    return item;
}



void QFPseudoTreeModel::clear()
{
    beginResetModel();
    m_folders.clear();
    QMapIterator<QString, QList<QFPseudoTreeModelItem*> > it(m_items);
    while (it.hasNext()) {
        it.next();
        qDeleteAll(it.value());
    }
    m_items.clear();
    endResetModel();
}


static bool QFPseudoTreeModelItem_lessthan(const QFPseudoTreeModelItem*s1, const QFPseudoTreeModelItem*s2)
{
    if (s1 && s2) {
        return s1->text()<s2->text();
    }
    return false;
}

void QFPseudoTreeModel::sort()
{
    beginResetModel();
    m_folders.sort();
    QMapIterator<QString, QList<QFPseudoTreeModelItem*> > it(m_items);
    while (it.hasNext()) {
        it.next();
        qSort(m_items[it.key()].begin(), m_items[it.key()].end(), QFPseudoTreeModelItem_lessthan);
    }
    endResetModel();
}



QFPseudoTreeModelItem::QFPseudoTreeModelItem(const QString &text)
{
    userData[Qt::DisplayRole] = text;
}

QFPseudoTreeModelItem::QFPseudoTreeModelItem(const QString &text, const QVariant &userRoleData)
{
    userData[Qt::DisplayRole] = text;
    userData[Qt::UserRole] = userRoleData;
}


QFPseudoTreeModelItem::QFPseudoTreeModelItem(QIcon icon, const QString &text, const QVariant &userRoleData)
{
    userData[Qt::DisplayRole] = text;
    userData[Qt::DecorationRole] = icon;
    userData[Qt::UserRole] = userRoleData;
}

QFPseudoTreeModelItem::~QFPseudoTreeModelItem()
{

}

int QFPseudoTreeModelItem::columnCount() const
{
    return 1;
}

QVariant QFPseudoTreeModelItem::displayText() const
{
    return data(Qt::DisplayRole);
}

QVariant QFPseudoTreeModelItem::data(int role) const
{
    return userData.value(role, QVariant());
}

QString QFPseudoTreeModelItem::text() const
{
    return userData.value(Qt::DisplayRole, QVariant()).toString();
}

void QFPseudoTreeModelItem::setData(QVariant data, int role)
{
    userData[role]=data;
}

void QFPseudoTreeModelItem::setIcon(const QIcon &icon)
{
    setData(icon, Qt::DecorationRole);
}

void QFPseudoTreeModelItem::setForeground(QColor color)
{
    setData(color, Qt::ForegroundRole);
    setData(color, Qt::TextColorRole);
}

void QFPseudoTreeModelItem::setBackground(QColor color)
{
    setData(color, Qt::BackgroundColorRole);
}

void QFPseudoTreeModelItem::setText(const QString &text)
{
   userData[Qt::DisplayRole] = text;
}






QFPseudoTreeModelSortFilterProxyModel::QFPseudoTreeModelSortFilterProxyModel(QObject *parent):
    QSortFilterProxyModel(parent)
{

}

bool QFPseudoTreeModelSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QFPseudoTreeModel* tree=qobject_cast<QFPseudoTreeModel*>(sourceModel());

    if (tree) {
        QModelIndex idx=tree->index(sourceRow, 0, sourceParent);
        if (idx.isValid()) {
            if (tree->isFolder(idx)) {
                QList<int> c=tree->folderContentsRows(idx);
                for (int i=0; i<c.size(); i++) {
                    if (filterAcceptsRow(c[i], sourceParent)) {
                        return true;
                    }
                }
                return false;
            } else {
                return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
            }
        }
    } else {
        return QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
    }
    return true;
}
