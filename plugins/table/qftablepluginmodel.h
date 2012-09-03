#ifndef QFTABLEPLUGINMODEL_H
#define QFTABLEPLUGINMODEL_H

#include "qftablemodel.h"

class QFTablePluginModel : public QFTableModel
{
    public:
        explicit QFTablePluginModel(QObject *parent=NULL);
        virtual QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

};

#endif // QFTABLEPLUGINMODEL_H
