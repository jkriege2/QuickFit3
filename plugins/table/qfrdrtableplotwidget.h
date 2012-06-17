#ifndef QFRDRTABLEPLOTWIDGET_H
#define QFRDRTABLEPLOTWIDGET_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"

namespace Ui {
    class QFRDRTablePlotWidget;
}

class QFRDRTablePlotWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRTablePlotWidget(QFRawDataPropertyEditor *propEditor, QFRawDataRecord* record, QWidget *parent = 0);
        ~QFRDRTablePlotWidget();
        
    private:
        Ui::QFRDRTablePlotWidget *ui;
};

#endif // QFRDRTABLEPLOTWIDGET_H
