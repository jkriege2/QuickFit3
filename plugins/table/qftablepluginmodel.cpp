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
