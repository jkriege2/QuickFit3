/*
Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-02-13 16:56:45 +0100 (Fr, 13 Feb 2015) $  (revision $Rev: 3827 $)

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

#ifndef QFGROUPANDROLEMODEL_H
#define QFGROUPANDROLEMODEL_H

#include <QAbstractTableModel>
#include "qfproject.h"
#include <QStyledItemDelegate>

class QFGroupAndRoleModel : public QAbstractTableModel
{
        Q_OBJECT
    public:
        explicit QFGroupAndRoleModel(QFProject* project, QObject *parent = 0);

        virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
        virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    signals:

    public slots:
        void setRoleAlwaysEditable(bool enabled);
        void setNameEditable(bool enabled);
    protected slots:
        void projectDataChanged();
    protected:
        QFProject* project;
        bool roleAlwaysEditable;
        bool nameEditable;

};



class QFGroupAndRoleDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        QFGroupAndRoleDelegate(QFProject* project, QObject* parent=NULL);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    private:
    protected slots:
        void currentIndexChanged(int index);
    protected:
        QFProject* project;
};

#endif // QFGROUPANDROLEMODEL_H
