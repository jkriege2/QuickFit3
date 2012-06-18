#ifndef QFRDRTABLEPLOTWIDGET_H
#define QFRDRTABLEPLOTWIDGET_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"

namespace Ui {
    class QFRDRTablePlotWidget;
}
class QFRDRTable;
class QFRDRTablePlotWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRTablePlotWidget(QFRDRTable* record, int plot, QWidget *parent = 0);
        ~QFRDRTablePlotWidget();

        void setRecord(QFRDRTable* record, int plot);
        
    private:
        Ui::QFRDRTablePlotWidget *ui;

        QFRDRTable* current;
        int plot;
        bool updating;
};

#endif // QFRDRTABLEPLOTWIDGET_H
