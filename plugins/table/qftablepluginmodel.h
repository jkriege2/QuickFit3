#ifndef QFTABLEPLUGINMODEL_H
#define QFTABLEPLUGINMODEL_H

#include "../base_classes/qftablemodel.h"

class QFTablePluginModel : public QFTableModel
{
    public:
       explicit QFTablePluginModel(QObject *parent=NULL);
};

#endif // QFTABLEPLUGINMODEL_H
