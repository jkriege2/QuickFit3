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
        QString exp=index.data(QFRDRTable::TableExpressionRole).toString();
        if (!exp.isEmpty()) {
            QString tt=QFTableModel::data(index, role).toString();
            tt+=tr("<font color=\"darkblue\"><br>expression: <i>%1</i></font>").arg(exp);
            return tt;
        }
    }
    return QFTableModel::data(index, role);
}


QVariant QFTablePluginModel::headerData(int section, Qt::Orientation orientation, int role) const {
    QVariant h=QFTableModel::headerData(section, orientation, role);
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
         return QString("%1: %2").arg(section+1).arg(h.toString());
     }

     return h;
}
