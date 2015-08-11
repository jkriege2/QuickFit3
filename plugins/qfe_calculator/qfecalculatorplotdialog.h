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

#ifndef QFECALCULATORPLOTDIALOG_H
#define QFECALCULATORPLOTDIALOG_H

#include <QDialog>
#include "qfplotter.h"

namespace Ui {
    class QFECalculatorPlotDialog;
}

class QFECalculatorPlotDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFECalculatorPlotDialog(QWidget *parent = 0);
        ~QFECalculatorPlotDialog();

        QFPlotter *getPlotter() const;
        void clearPlots();
        void startAddingPlots();
        void endAddingPlots();
        void setLog(bool logX, bool logY);
        void setAxisLabel(const QString &labX, const QString &labY);
        void setLogX(bool log);
        void setLogY(bool log);
        void setGrid(bool enabled, const QString &style=QString());
        void setXAxisLabel(const QString& label);
        void setYAxisLabel(const QString& label);
        void addPlot(const QVector<double>& X, const QVector<double>& Y, const QString& spec=QString(), const QString& label=QString());
        void addBarPlot(const QVector<double>& X, const QVector<double>& Y, const QString& spec=QString(), const QString& label=QString());
        void addErrorPlot(const QVector<double>& X, const QVector<double>& Y, const QVector<double>& Xerrors, const QVector<double>& Yerrors, const QString& spec=QString(), const QString& label=QString(), JKQTPerrorPlotstyle errorstyle=JKQTPerrorBars);
        void addScatterPlot(const QVector<double>& X, const QVector<double>& Y, const QVector<double>& S, const QString& spec=QString(), const QString& label=QString());
        void addColorScatterPlot(const QVector<double>& X, const QVector<double>& Y, const QVector<double>& S, const QString& spec=QString(), const QString& label=QString(), const QString& colorscale=QString("matlab"));
        void fitRanges();

    protected slots:
        void showHelp();
        void closeClicked();
        void updatePlot();

        void on_chkLogX_toggled(bool checked);
        void on_chkLogY_toggled(bool checked);
        void on_chkGrid_toggled(bool checked);

    private:
        Ui::QFECalculatorPlotDialog *ui;
};

#endif // QFECALCULATORPLOTDIALOG_H

