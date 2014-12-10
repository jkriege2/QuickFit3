/*
Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

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

#ifndef QFRDRIMAGINGFCSMASKBUILDER_H
#define QFRDRIMAGINGFCSMASKBUILDER_H

#include <QDialog>

namespace Ui {
    class QFRDRImagingFCSMaskBuilder;
}

class QFRDRImagingFCSMaskBuilder : public QDialog
{
        Q_OBJECT

    public:
        explicit QFRDRImagingFCSMaskBuilder(const QStringList &parameters, const QStringList &parameterNames, QWidget *parent = 0);
        ~QFRDRImagingFCSMaskBuilder();

        bool use1(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool use2(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool use3(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool use4(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool use5(QString& parameter, int& mode, double& thresh1, double& thresh2);
        bool onlyMasked() const;
        bool saveSelection() const;
        QString selName() const;
        bool applyToOtherRDRs() const;

        void enableSelectionName(bool en);

    private:
        Ui::QFRDRImagingFCSMaskBuilder *ui;
};

#endif // QFRDRIMAGINGFCSMASKBUILDER_H
