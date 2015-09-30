/*
Copyright (c) 2012-2015 by Sebastian Isbaner

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

#ifndef BURSTSTATISTICSVIEW_H
#define BURSTSTATISTICSVIEW_H

#include <QWidget>
#include "alexdata.h"
#include "jkqtplotter.h"

namespace Ui {
class BurstStatisticsView;
}

class BurstStatisticsView : public QWidget
{
    Q_OBJECT
    
public:
    explicit BurstStatisticsView(QWidget *parent = 0, AlexData* data = NULL);
    ~BurstStatisticsView();

    /// store the settings of this widget in \a settings with prefix \a prefix (e.g. group)
    void storeSettings(QSettings& settings, const QString& prefix=QString("")) const;
    /// load the widget's settings from \a settings. the \a prefix must be the same as for storeSettings
    void loadSettings(QSettings& settings, const QString& prefix=QString(""));

    /// calculates burst statistics and plots burst duration distribution, burst size distribution and burst rate distribution
    void evaluate();
    

private:
    Ui::BurstStatisticsView *ui;
    AlexData* ad;
    FRETparams_t rangeTgTr;
    void plotStatistics();
    void plotTgTr();

private slots:
    void on_pushButtonRefresh_clicked();
    void on_spinBoxBins_editingFinished();
};

#endif // BURSTSTATISTICSVIEW_H
