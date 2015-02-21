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

#ifndef QFRDRTABLEAXISSETTINGSWIDGET_H
#define QFRDRTABLEAXISSETTINGSWIDGET_H

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
    class QFRDRTableAxisSettingsWidget;
}
class QFRDRTableAxisSettingsWidget : public QWidget
{
        Q_OBJECT
        
    public:
        explicit QFRDRTableAxisSettingsWidget(QWidget *parent);
        ~QFRDRTableAxisSettingsWidget();

        void loadPlotData(const QFRDRTable::AxisInfo &g, bool overwriteLabels=true);
        void storePlotData(QFRDRTable::AxisInfo &g);

        bool getLogX() const;

        
        /** \brief read the settings */
        virtual void readSettings(QSettings& settings, const QString& prefix=QString(""));
        /** \brief write the settings */
        virtual void writeSettings(QSettings& settings, const QString& prefix=QString(""));
        void saveToSettings(QSettings& settings, const QString& axis) const;
        void loadFromSettings(QSettings& settings, const QString& axis);
        QString getlabel() const;
        void setRecord(QFRDRTable *record, int graph);

    public slots:
        void setXRange(double xmin, double xmax);
        void setlabel(const QString& label);
        void doUpdateGraph();

        void connectWidgets();
        void disconnectWidgets();

    signals:
        void updateGraph();
        void autoscaleX();
        void dataChanged();
    protected slots:
        void on_btnAutoscaleX_clicked();
        void plotDataChanged();
        void updateComboboxes();

    protected:

        void reloadColumns(QComboBox *combo);
    private:
        Ui::QFRDRTableAxisSettingsWidget *ui;
        QFRDRTable* current;
        int plot;
        bool updating;
};

#endif // QFRDRTABLEAXISSETTINGSWIDGET_H
