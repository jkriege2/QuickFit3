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

#ifndef QFRDRIMAGINGFCSSIMULATOR_H
#define QFRDRIMAGINGFCSSIMULATOR_H

#include <QDialog>
#include "qfrdrimagingfcssimulationthread.h"

namespace Ui {
    class QFRDRImagingFCSSimulator;
}

class QFRDRImagingFCSSimulator : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFRDRImagingFCSSimulator(QWidget *parent = 0);
        ~QFRDRImagingFCSSimulator();
        
        QString getSimulationFilename() const;
    protected slots:
        void on_btnHelp_clicked();
        void writeSettings() const;
        void readSettings();
        void on_btnRun_clicked();
        void on_btnCancel_clicked();
        void threadFinished();
    private:
        Ui::QFRDRImagingFCSSimulator *ui;

        enum DialogState {
            dsParameterInput=0,
            dsRunning=1,
            dsFinished=2
        };

        DialogState dstate;

        void setState(DialogState new_state);

        QString lastSimFile;

        QFRDRImagingFCSSimulationThread* sim;
};

#endif // QFRDRIMAGINGFCSSIMULATOR_H
