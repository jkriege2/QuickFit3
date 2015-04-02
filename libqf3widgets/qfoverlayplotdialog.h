/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QFOVERLAYPLOTDIALOG_H
#define QFOVERLAYPLOTDIALOG_H

#include <QDialog>
#include "qfgetplotdatainterface.h"
#include "libwid_imexport.h"

class QFPlotter; // forward
class QFMatchRDRFunctor;

namespace Ui {
    class QFOverlayPlotDialog;
}

class QFWIDLIB_EXPORT QFOverlayPlotDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit QFOverlayPlotDialog(QWidget *parent = 0);
        ~QFOverlayPlotDialog();

        QFPlotter* getPlotter() const;
        void clearPlots();
        void startAddingPlots();
        void addPlot(const QVector<double>& x, const QVector<double>& y, const QString& name, const QVector<double>& xerror=QVector<double>(), const QVector<double>& yerror=QVector<double>());
        void addPlot(const QFGetPlotdataInterface::GetPlotDataItem& plotData);
        void setPlotOptions(const QFGetPlotdataInterface::GetPlotPlotOptions& options);
        void endAddingPlots();
        void setLog(bool logX=false, bool logY=false);
        void setAxisLabel(const QString& labX, const QString& labY);

        static QFOverlayPlotDialog *collectOverlayPlot(QFMatchRDRFunctor* matchFunctor);
    protected slots:
        void showHelp();
        void closeClicked();
        void updatePlot();
    private:
        Ui::QFOverlayPlotDialog *ui;
};

#endif // QFOVERLAYPLOTDIALOG_H
