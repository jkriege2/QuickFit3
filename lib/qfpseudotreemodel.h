/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-07 20:51:17 +0100 (Sa, 07 Feb 2015) $  (revision $Rev: 3802 $)

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
#ifndef QFPSEUDOTREEMODEL_H
#define QFPSEUDOTREEMODEL_H

#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "lib_imexport.h"
#include <QList>
#include <QSortFilterProxyModel>
#include <QIcon>
#include <QColor>

class QFPseudoTreeModelItem; // forward

class QFLIB_EXPORT QFPseudoTreeModel : public QAbstractListModel
{
        Q_OBJECT
    public:
        explicit  QFPseudoTreeModel(QObject *parent = 0);
        ~QFPseudoTreeModel();

        QVariant data(const QModelIndex &index, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        QModelIndex index(int row, int column,  const QModelIndex &parent = QModelIndex()) const;
        QModelIndex index(const QFPseudoTreeModelItem* item) const;
        QModelIndex index(QVariant data, int role) const;
        QModelIndex parent(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        int columnCount(const QModelIndex &parent = QModelIndex()) const;
        QFPseudoTreeModelItem* itemForIndex(const QModelIndex& index) const;
        int folderIndexForIndex(const QModelIndex& index) const;
        bool isFolder(const QModelIndex& index) const;
        QList<int> folderContentsRows(int folder) const;
        QList<int> folderContentsRows(const QModelIndex& folder) const;
        bool isLastInFolder(const QModelIndex& index) const;

        QFPseudoTreeModelItem* addFolderedItem(const QString& name, const QVariant& userData, QChar separator=QLatin1Char('/'));
        QFPseudoTreeModelItem* addFolderedItem(const QString& folder, const QString& name, const QVariant& userData);

        void clear();

        void sort();

    protected:
        QStringList m_folders;
        QMap<QString, QList<QFPseudoTreeModelItem*> > m_items;

};



class QFLIB_EXPORT QFPseudoTreeModelItem {
public:
    QFPseudoTreeModelItem(const QString &text);
    QFPseudoTreeModelItem(const QString &text, const QVariant& userRoleData);
    QFPseudoTreeModelItem(QIcon icon, const QString &text, const QVariant& userRoleData);
    ~QFPseudoTreeModelItem();

    int columnCount() const;
    QVariant displayText() const;
    QVariant data(int role) const;
    QString text() const;
    void setData(QVariant data, int role=Qt::UserRole);
    void setIcon(const QIcon& icon);
    void setForeground(QColor color);
    void setBackground(QColor color);
    void setText(const QString& text);

private:
    QMap<int, QVariant> userData;
};


class QFLIB_EXPORT QFPseudoTreeModelSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    QFPseudoTreeModelSortFilterProxyModel(QObject *parent = 0);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

};
#endif // QFPSEUDOTREEMODEL_H
