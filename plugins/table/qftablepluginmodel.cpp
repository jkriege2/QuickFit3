/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    

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
        QString cexp=headerData(index.column(), Qt::Horizontal, QFRDRTable::ColumnExpressionRole).toString();
        QString exp=index.data(QFRDRTable::TableExpressionRole).toString();
        QString comment=index.data(QFRDRTable::TableCommentRole).toString();
        if (!exp.isEmpty()) {
            tt+=tr("<font color=\"darkblue\"><br>expression: <i>%1</i></font>").arg(qfHTMLEscape(exp));
        }
        if (!cexp.isEmpty()) {
            tt+=tr("<font color=\"darkblue\"><br>column expression: <i>%1</i></font>").arg(qfHTMLEscape(cexp));
        }
        if (!comment.isEmpty()) {
            tt+=tr("<font color=\"black\"><br>comment: <i>%1</i></font>").arg(qfHTMLEscape(comment));
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
                return h.toString()+tr("<font color=\"darkblue\"><br>column expression: <i>%1</i></font>").arg(qfHTMLEscape(exp));
            }
        }
    }

     return h;
}


//QFTablePluginExpressionsModel::QFTablePluginExpressionsModel(QFTablePluginModel *model, QObject *parent):
//    QAbstractTableModel(parent)
//{
//    this->model=model;
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
//    beginResetModel();
//    endResetModel();
//#else
//    reset();
//#endif
//}

//Qt::ItemFlags QFTablePluginExpressionsModel::flags(const QModelIndex &index) const
//{
//    int col=index.column();
//    int row=index.row();
//    /*if (project) {
//        QFRawDataRecord* rdr=project->getRawDataByNum(row);
//        if (rdr) {
//            if (col==0) { // name
//                if (nameEditable) return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
//                else return Qt::ItemIsSelectable|Qt::ItemIsEnabled;

//            } else if (col==1) { // group
//                return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;

//            } else if (col==2) { // role
//                if (rdr->isRoleUserEditable() || roleAlwaysEditable) return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
//                else return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
//            } else if (col==3) { // folder
//                return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
//            }
//        }
//    }*/
//    return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
//}

//QVariant QFTablePluginExpressionsModel::data(const QModelIndex &index, int /*role*/) const
//{
//    int col=index.column();
//    int row=index.row();
//    if (model) {

//    }
//    return QVariant();
//}

//QVariant QFTablePluginExpressionsModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
//{
//    if (model) {

//    }
//    return QVariant();
//}

//int QFTablePluginExpressionsModel::rowCount(const QModelIndex &/*parent*/) const
//{
//    return 0;
//}

//int QFTablePluginExpressionsModel::columnCount(const QModelIndex &/*parent*/) const
//{
//    return 0;
//}

//bool QFTablePluginExpressionsModel::setData(const QModelIndex &/*index*/, const QVariant &/*value*/, int /*role*/)
//{
//    return false;
//}


//QFTablePluginExpressionsModelDelegate::QFTablePluginExpressionsModelDelegate(QObject *parent):
//    QStyledItemDelegate(parent)
//{

//}

//QWidget *QFTablePluginExpressionsModelDelegate::createEditor(QWidget */*parent*/, const QStyleOptionViewItem &/*option*/, const QModelIndex &/*index*/) const
//{
//    int col=index.column();
//    int row=index.row();

//    return NULL;
//}

//void QFTablePluginExpressionsModelDelegate::setEditorData(QWidget */*editor*/, const QModelIndex &index) const
//{

//}

//void QFTablePluginExpressionsModelDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
//{

//}

//void QFTablePluginExpressionsModelDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
//{
//    QStyledItemDelegate::updateEditorGeometry(editor, option, index);
//}
