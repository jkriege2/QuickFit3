/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

    last modification: $LastChangedDate: 2014-06-22 12:12:54 +0200 (So, 22 Jun 2014) $  (revision $Rev: 3280 $)

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

#ifndef QFFITALGORITHMERRORESTIMATEMODEWIDGET_H
#define QFFITALGORITHMERRORESTIMATEMODEWIDGET_H

#include <QWidget>
#include "qffitalgorithm.h"
#include "qfproperties.h"
#include "lib_imexport.h"

namespace Ui {
    class QFFitAlgorithmErrorEstimateModeWidget;
}

class QFLIB_EXPORT QFFitAlgorithmErrorEstimateModeWidget : public QWidget
{
        Q_OBJECT

    public:
        explicit QFFitAlgorithmErrorEstimateModeWidget(QWidget *parent = 0);
        ~QFFitAlgorithmErrorEstimateModeWidget();

        QFFitAlgorithm::FitParameterErrorEstimates getMode() const;
        void setMode(QFFitAlgorithm::FitParameterErrorEstimates mode);

        void setBootstrappingRepeats(int repeats);
        int getBootstrappingRepeats() const;
        void setBootstrappingDistortion(double value);
        double getBootstrappingDistortion() const;
        void setBootstrappingFraction(double value);
        double getBootstrappingFraction() const;

        QString toString() const;

        void saveSettings(QFProperties* props, const QString& prefix=QString()) const;
        void readSettings(QFProperties* props, const QString& prefix=QString());
    signals:
        void parametersChanged();
        void modeChanged(QFFitAlgorithm::FitParameterErrorEstimates mode);
    protected slots:
        void on_cmbMode_currentIndexChanged(int index);
        void on_spinBSRepeats_valueChanged(int value);
        void on_spinBSDistortion_valueChanged(double value);
        void on_spinBSFraction_valueChanged(double value);
    private:
        Ui::QFFitAlgorithmErrorEstimateModeWidget *ui;
};

#endif // QFFITALGORITHMERRORESTIMATEMODEWIDGET_H
