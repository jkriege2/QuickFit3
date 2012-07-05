#ifndef QFRDRTABLEPLOTWIDGET_H
#define QFRDRTABLEPLOTWIDGET_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"
#include <cmath>

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

        void rawDataChanged();
        
        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));
    protected slots:
        void listGraphs_currentRowChanged(int currentRow);
        void on_btnDeleteGraph_clicked();
        void on_btnAddGraph_clicked();

        void reloadColumns(QComboBox* combo);
        void graphDataChanged();
        void plotDataChanged();
        void updateGraph();
        void updateData();

        void connectWidgets();
        void disconnectWidgets();

    private:
        Ui::QFRDRTablePlotWidget *ui;

        QFRDRTable* current;
        int plot;
        bool updating;
};

#endif // QFRDRTABLEPLOTWIDGET_H
