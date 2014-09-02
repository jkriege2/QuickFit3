/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

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

#include "qftablepluginmodel.h"
#include "qfrdrtable.h"

QFTablePluginModel::QFTablePluginModel(QObject* parent):
    QFTableModel(parent)
{
    setVerticalHeaderShowRowNumbers(true);
}


QVariant QFTablePluginModel::data(const QModelIndex &index, int role) const
{
    if (role==Qt::ToolTipRole) {
        QString tt=QFTableModel::data(index, role).toString();
        QString exp=index.data(QFRDRTable::TableExpressionRole).toString();
        QString comment=index.data(QFRDRTable::TableCommentRole).toString();
        if (!exp.isEmpty()) {
            tt+=tr("<font color=\"darkblue\"><br>expression: <i>%1</i></font>").arg(exp);
        }
        if (!comment.isEmpty()) {
            tt+=tr("<font color=\"black\"><br>comment: <i>%1</i></font>").arg(comment);
        }
        return tt;
    }
    return QFTableModel::data(index, role);
}

Qt::ItemFlags QFTablePluginModel::flags(const QModelIndex &index) const
{
    if (index.model()) {
        QVariant colexp=index.model()->headerData(index.column(), Qt::Horizontal, QFRDRTable::ColumnExpressionRole);
        if (colexp.isValid() && colexp.canConvert(QVariant::String)) {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }
    return QFTableModel::flags(index);
}


QVariant QFTablePluginModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant h=QFTableModel::headerData(section, orientation, role);
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return QString("%1: %2").arg(section+1).arg(h.toString());
    }
    if (orientation == Qt::Horizontal && role == Qt::ToolTipRole) {
        if (this->hasColumnHeaderData(section, QFRDRTable::ColumnExpressionRole)) {
            QString exp=this->getColumnHeaderData(section, QFRDRTable::ColumnExpressionRole).toString();
            if (!exp.isEmpty()) {
                return h.toString()+tr("<font color=\"darkblue\"><br>column expression: <i>%1</i></font>").arg(exp);
            }
        }
    }

     return h;
}
