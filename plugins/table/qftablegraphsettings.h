/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg



    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef QFTABLEGRAPHSETTINGS_H
#define QFTABLEGRAPHSETTINGS_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"
#include <cmath>
#include "cpptools.h"
#include "qfrdrtable.h"
#include "QToolTip"
#include "qfmathparser.h"
#include "qffunctionreferencetool.h"
#include "qffitfunctionvalueinputtable.h"

namespace Ui {
    class QFTableGraphSettings;
}

class QFTableGraphSettings : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFTableGraphSettings(QWidget *parent = 0);
        ~QFTableGraphSettings();
        void setRecord(QFRDRTable* record, int plot);

        void rawDataChanged();

        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));

        void writeGraphData(QFRDRTable::GraphInfo &graph);
        void loadGraphData(const QFRDRTable::GraphInfo &graph);
        void updatePlotWidgetVisibility();
        QIcon getGraphIcon(int i) const;
    signals:
        void graphDataChanged();
        void performFit(int xCol, int yCol, int sigmaCol, int plot, QString function, QFRDRTable::GraphDataSelection sel);
        void performRegression(int xCol, int yCol, int sigmaCol, int  plot, QFRDRTable::GraphDataSelection sel);
        void performRefit(int plot);
        void reloadGraph();
    public slots:
        void initFocus();

    protected slots:
        void on_btnFunctionHelp_clicked();
        void on_edtFunction_textChanged(const QString &text);
        void on_btnAutoX_clicked();
        void on_btnAutoY_clicked();

        void on_btnShiftAuto_clicked();
        void on_cmbSelectDataCompare_currentIndexChanged(int index);
        void on_cmbRangeMode_currentIndexChanged(int index);


        void on_btnClearLinesXData_clicked();
        void on_btnClearLinesXError_clicked();
        void on_btnClearLinesXError2_clicked();
        void on_btnClearLinesYData_clicked();
        void on_btnClearLinesYError_clicked();
        void on_btnClearLinesYError2_clicked();
        void on_btnClearLinesMax_clicked();
        void on_btnClearLinesMean_clicked();
        void on_btnClearLinesQ75_clicked();
        void on_cmbSelectDataLogic12_currentIndexChanged(int index);
        void on_cmbSelectDataLogic23_currentIndexChanged(int index);
        void updateSelectDataEnabled();

        void cmbFunctionTypeCurrentIndexChanged(int index);

        void on_btnAutoImageSizes_clicked();

        void writeGraphData();
        void reloadColumns(QComboBox* combo);

        void connectWidgets();
        void disconnectWidgets();

        void doFit();
        void doRegression();
        void doRefit();

        void updateComboboxes();
        void fitFunctionChanged();
    protected:

    private:
        QFTableModelColumnHeaderModel* headerModel;
        //QStringListModel* headerModel;
        Ui::QFTableGraphSettings *ui;
        QFRDRTable* current;
        int plot;
        bool updating;
        QFFunctionReferenceTool* functionRef;

        QFFitFunctionValueInputTable* fitfuncValuesTable;
        QVector<double> fitfuncValues, fitfuncValuesDummy;
        QVector<double> fitfuncErrors, fitfuncErrorsDummy;

        QAction* actFit;
        QAction* actRegression;

        bool isFitResult;
};

#endif // QFTABLEGRAPHSETTINGS_H
