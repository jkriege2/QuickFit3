/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFTABLEPLUGINMODEL_H
#define QFTABLEPLUGINMODEL_H

#include "qftablemodel.h"
#include <QStyledItemDelegate>
#include <QAbstractTableModel>

class QFTablePluginModel : public QFTableModel
{
        Q_OBJECT
    public:
        explicit QFTablePluginModel(QObject *parent=NULL);
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex &index) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};



/*
class QFTablePluginExpressionsModel : public QAbstractTableModel
{
        Q_OBJECT
    public:
        explicit QFTablePluginExpressionsModel(QFTablePluginModel* model, QObject *parent = 0);

        virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
        virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
        virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
        virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
        virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    signals:

    public slots:

    protected slots:

    protected:
        QFTablePluginModel* model;

};



class QFTablePluginExpressionsModelDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        QFTablePluginExpressionsModelDelegate(QObject* parent=NULL);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void setEditorData(QWidget *editor, const QModelIndex &index) const;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    private:
    protected slots:
    protected:

};*/
#endif // QFTABLEPLUGINMODEL_H
