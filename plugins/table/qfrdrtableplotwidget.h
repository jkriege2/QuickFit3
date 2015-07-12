/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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

#ifndef QFRDRTABLEPLOTWIDGET_H
#define QFRDRTABLEPLOTWIDGET_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"
#include <cmath>
#include "cpptools.h"
#include "qfrdrtable.h"
#include "QToolTip"
#include "qfmathparser.h"
#include "qffunctionreferencetool.h"

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


        double getMagnification() const;
    signals:
        void plotTitleChanged(int plot, QString title);
    signals:
        void performFit(int xCol, int yCol, int sigmaCol, int plot, int graph, QString function, QFRDRTable::GraphDataSelection sel,bool xlog,bool ylog);
        void performRegression(int xCol, int yCol, int sigmaCol, int plot,int graph, QFRDRTable::GraphDataSelection sel,bool xlog,bool ylog);
        void performRefit(int plot, int graph);
    protected slots:
        void doFit(int xCol, int yCol, int sigmaCol, int plot, QString function, QFRDRTable::GraphDataSelection sel);
        void doRegression(int xCol, int yCol, int sigmaCol, int plot, QFRDRTable::GraphDataSelection sel);
        void doRefit(int plot);
        void listGraphs_currentRowChanged(int currentRow);
        void on_btnDeleteGraph_clicked();
        void on_btnCloneGraph_clicked();
        void on_btnAddGraph_clicked();
        void doAutoscaleX();
        void doAutoscaleY();
        void on_plotter_plotMouseMove(double x, double y);
        void on_btnMoveUp_clicked();
        void on_btnMoveDown_clicked();
        void on_btnResetColoring_clicked();
        void on_btnColorByPalette_clicked();
        void reloadGraphData();

        void graphDataChanged();
        void plotDataChanged();
        void updateGraph();
        void updateData();
        void updatePlotWidgetVisibility();

        void connectWidgets();
        void disconnectWidgets();
        void magnificationChanged(int idx);
    protected:
        int getColumnWithStride(int column, const QFRDRTable::GraphInfo &g);
        void autoColorGraph(QFRDRTable::GraphInfo& g, int autocolor=0);
        void autoColorGraph(QFRDRTable::GraphInfo& g, QColor color);


        void setAxisProps(JKQTPcoordinateAxis *axis, const QFRDRTable::AxisInfo &axisData, const QFRDRTable::PlotInfo &p, bool minorGrid=true, bool majorGrid=true);
    private:
        Ui::QFRDRTablePlotWidget *ui;
        QToolBar* toolbarPlot;
        QLabel* labPlotPosition;

        QFRDRTable* current;
        int plot;
        bool updating;
        QList<QColor> autocolors;
        QComboBox* cmbMagnification;
};

#endif // QFRDRTABLEPLOTWIDGET_H
