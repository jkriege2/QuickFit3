#ifndef QFRDRTABLEPLOTWIDGET_H
#define QFRDRTABLEPLOTWIDGET_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"
#include <cmath>
#include "tools.h"
#include "qfrdrtable.h"

namespace Ui {
    class QFRDRTablePlotWidget;
}
class QFRDRTablePlotWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRTablePlotWidget(QWidget *parent);
        ~QFRDRTablePlotWidget();

        void setRecord(QFRDRTable* record, int plot);

        void rawDataChanged();
        
        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));
    signals:
        void plotTitleChanged(int plot, QString title);
    protected slots:
        void listGraphs_currentRowChanged(int currentRow);
        void on_btnDeleteGraph_clicked();
        void on_btnCloneGraph_clicked();
        void on_btnAddGraph_clicked();
        void on_btnAutoscaleX_clicked();
        void on_btnAutoscaleY_clicked();
        void on_plotter_plotMouseMove(double x, double y);
        void on_btnLoadSystem_clicked();
        void on_btnSaveSystem_clicked();
        void on_btnMoveUp_clicked();
        void on_btnMoveDown_clicked();
        void on_btnResetColoring_clicked();

        void reloadColumns(QComboBox* combo);
        void graphDataChanged();
        void plotDataChanged();
        void updateGraph();
        void updateData();
        void updatePlotWidgetVisibility();

        void connectWidgets();
        void disconnectWidgets();
    protected:
        int getColumnWithStride(int column, const QFRDRTable::GraphInfo &g);
        void autoColorGraph(QFRDRTable::GraphInfo& g, int autocolor=0);

    private:
        Ui::QFRDRTablePlotWidget *ui;
        QToolBar* toolbarPlot;
        QLabel* labPlotPosition;

        QFRDRTable* current;
        int plot;
        bool updating;
        QList<QColor> autocolors;
};

#endif // QFRDRTABLEPLOTWIDGET_H
