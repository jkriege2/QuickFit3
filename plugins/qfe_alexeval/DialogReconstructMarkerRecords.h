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

#ifndef DIALOGRECONSTRUCTMARKERRECORDS_H
#define DIALOGRECONSTRUCTMARKERRECORDS_H

#include "qfdialog.h"

namespace Ui {
class DialogReconstructMarkerRecords;
}

enum ReconstructionMethod {
    ReconstructFromDonor,
    ReconstructFromAcceptor,
    FullConstruction,
    SingleWavelengthFRET
};

/**
 * @brief The DialogReconstructMarkerRecords class
 *The user can choose between two methods: reconstruction from a single marker channel with an offset
 * or full construction. For the first case, the period for checkMarker may be supplied manually.
 * usage: you can use setPeriod, setStartTime, setOffset to set new preset values.
 * You should use setInfoText to set the number of marker events
 * (otherwise false information will be displayed)
 * After calling exec(), you can acquire the user settings via the getter methods.
 */
class DialogReconstructMarkerRecords : public QFDialog
{
    Q_OBJECT

public:
    explicit DialogReconstructMarkerRecords(QWidget *parent = 0);
    ~DialogReconstructMarkerRecords();

    double getPeriod(); // in seconds
    void setPeriod(double period);
    double getOffset();
    void setOffset(double offset);
    ReconstructionMethod getChannel();
    void setChannel( ReconstructionMethod channel );
    void setInfoText(int nEventsD, int nEventsA, QString additionalInfo=QString(""));

    double getFullDonorPeriod();
    double getFullStartTime();
    void setFullStartTime(double startTime);
    double getFullOffset();

private slots:
    void on_radioButtonReconstruct_toggled(bool checked);
    void on_radioButtonFullConstruct_toggled(bool checked);
    void on_radioButtonFRET_toggled(bool checked);

    void setLabelTextChannel(QString channel);
    bool checkPossibleReconstruction();

private:
    Ui::DialogReconstructMarkerRecords *ui;
    int nMarkerEventsD;
    int nMarkerEventsA;

};

#endif // DIALOGRECONSTRUCTMARKERRECORDS_H
