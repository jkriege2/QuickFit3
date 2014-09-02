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

#ifndef QFRDRIMAGINGFCSSERIESDIALOG_H
#define QFRDRIMAGINGFCSSERIESDIALOG_H
#include <QList>
#include <QDialog>
#include <QSettings>
#include "qfpluginservices.h"
#include "programoptions.h"

namespace Ui {
    class QFRDRImagingFCSSeriesDialog;
}

class QFRDRImagingFCSSeriesDialog : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRImagingFCSSeriesDialog(QWidget *parent = 0);
        ~QFRDRImagingFCSSeriesDialog();
        int getParameter() const;
        QList<double> getValues() const;
        void setParameter(const QString& parameter);
        void setParameter(int idx);
        void setRange(double start, double end, double inc);
        
    private:
        Ui::QFRDRImagingFCSSeriesDialog *ui;

        void closeEvent(QCloseEvent *event);
};

#endif // QFRDRIMAGINGFCSSERIESDIALOG_H
