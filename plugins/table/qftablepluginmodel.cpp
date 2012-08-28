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
