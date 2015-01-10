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

#ifndef QFRDRTABLEPLOTSETTINGSWIDGET_H
#define QFRDRTABLEPLOTSETTINGSWIDGET_H

#include <QWidget>
#include "qfrawdatapropertyeditor.h"
#include "qfrawdatarecord.h"
#include <cmath>
#include "tools.h"
#include "qfrdrtable.h"
#include "QToolTip"
#include "qfmathparser.h"
#include "qffunctionreferencetool.h"

namespace Ui {
    class QFRDRTablePlotSettingsWidget;
}
class QFRDRTablePlotSettingsWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRTablePlotSettingsWidget(QWidget *parent);
        ~QFRDRTablePlotSettingsWidget();

        void setRecord(QFRDRTable* record, int plot);

        bool getLogX() const;
        bool getLogY() const;

        
        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));
    public slots:
        void setXRange(double xmin, double xmax);
        void setYRange(double ymin, double ymax);
        void graphDataChanged(int r);
        void plotDataChanged();
        void doUpdateGraph();

        void connectWidgets();
        void disconnectWidgets();

    signals:
        void plotTitleChanged(int plot, QString title);
        void updateGraph();
        void autoscaleX();
        void autoscaleY();
        void plotSettingsChanged();
    protected slots:
        void on_btnLoadSystem_clicked();
        void on_btnSaveSystem_clicked();
        void on_btnCopySystem_clicked();
        void on_btnPasteSystem_clicked();
        void on_btnAutoscaleXY_clicked();
        void doAutoscaleX();
        void doAutoscaleY();

    protected:

    private:
        Ui::QFRDRTablePlotSettingsWidget *ui;

        QFRDRTable* current;
        int plot;
        bool updating;
};

#endif // QFRDRTABLEPLOTSETTINGSWIDGET_H
