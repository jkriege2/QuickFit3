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

#ifndef QFSIMPLETREEMODEL_H
#define QFSIMPLETREEMODEL_H

#include <QAbstractItemModel>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "lib_imexport.h"
#include <QList>
#include <QVariant>

class  QFSimpleTreeModelItem; // forward
/*! \brief this class implements a very simple tree model for Qt's model-view-framework, which may contain nodes (selectable) and selectable, as well as non-selectable folders
    \ingroup tools_qt


 */
class QFLIB_EXPORT  QFSimpleTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QFSimpleTreeModel(QObject *parent = 0);
    ~QFSimpleTreeModel();
    
     QVariant data(const QModelIndex &index, int role) const;
     Qt::ItemFlags flags(const QModelIndex &index) const;
     QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
     QModelIndex index(int row, int column,  const QModelIndex &parent = QModelIndex()) const;
     QModelIndex parent(const QModelIndex &index) const;
     int rowCount(const QModelIndex &parent = QModelIndex()) const;
     int columnCount(const QModelIndex &parent = QModelIndex()) const;

     QFSimpleTreeModelItem* addFolderedItem(const QString& name, const QVariant& userData, QChar separator=QLatin1Char('/'));

     void clear();

 private:
     void setupModelData(const QStringList &lines, QFSimpleTreeModelItem *parent);

     QFSimpleTreeModelItem *rootItem;

};









 class QFLIB_EXPORT QFSimpleTreeModelItem {
 public:
         QFSimpleTreeModelItem(const QString &text, QFSimpleTreeModelItem *parent = 0);
         QFSimpleTreeModelItem(const QString &text, const QVariant& userRoleData, QFSimpleTreeModelItem *parent = 0);
         QFSimpleTreeModelItem(bool folder, const QString &text, QFSimpleTreeModelItem *parent = 0);
         QFSimpleTreeModelItem(bool folder, const QString &text, const QVariant& userRoleData, QFSimpleTreeModelItem *parent = 0);
     QFSimpleTreeModelItem(QIcon icon, const QString &text, const QVariant& userRoleData, QFSimpleTreeModelItem *parent = 0);
     ~QFSimpleTreeModelItem();

     void appendChild(QFSimpleTreeModelItem *child);

     QFSimpleTreeModelItem *child(int row);
     int childCount() const;
     int columnCount() const;
     QVariant displayText() const;
     QVariant data(int role) const;
     void setData(QVariant data, int role=Qt::UserRole);
     int row() const;
     QFSimpleTreeModelItem *parent();
     bool isFolder() const;
     void setFolder(bool folder);

 private:
     QList<QFSimpleTreeModelItem*> childItems;
     QMap<int, QVariant> userData;
     QFSimpleTreeModelItem *parentItem;
     bool folder;
 };

#endif // QFSIMPLETREEMODEL_H
