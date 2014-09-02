/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center

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

#ifndef QFFITALGORITHMLMFITCONFIG_H
#define QFFITALGORITHMLMFITCONFIG_H

#include <QDialog>

namespace Ui {
    class QFFitAlgorithmLMFitConfigDialog;
}

/*! \brief dialog class for lmfit Fit Algorithm
    \ingroup qf3fitalgp_lmfit
*/
class QFFitAlgorithmLMFitConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QFFitAlgorithmLMFitConfigDialog(QWidget *parent = 0, bool IRLS=false);
    ~QFFitAlgorithmLMFitConfigDialog();

    void init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations);
    void init(double ftol, double xtol, double gtol, double epsilon, double stepbound, int max_iterations, double irls_parameter, int irls_iterations);

    double ftol() const;
    double xtol() const;
    double gtol() const;
    double epsilon() const;
    double stepbound() const;
    int max_iterations() const;
    int irls_iterations() const;
    double irls_parameter() const;
private:
    Ui::QFFitAlgorithmLMFitConfigDialog *ui;
};

#endif // QFFITALGORITHMLMFITCONFIG_H
