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

#ifndef QFFCSMSDEVALUATIONGETNFROMFITS_H
#define QFFCSMSDEVALUATIONGETNFROMFITS_H

#include <QDialog>
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
namespace Ui {
    class QFFCSMSDEvaluationGetNFromFits;
}

class QFFCSMSDEvaluationGetNFromFits : public QDialog
{
        Q_OBJECT
        
    public:
        explicit QFFCSMSDEvaluationGetNFromFits(QFRawDataRecord* current, QWidget *parent = 0);
        ~QFFCSMSDEvaluationGetNFromFits();

        int getApplyTo() const;
        QString getEvalGroup() const;
        QString getParameterN() const;
        QString getParameterTripletTheta() const;
        QString getParameterTripletTau() const;
        QString getParameterDarkTheta() const;
        QString getParameterDarkTau() const;
        QString getParameterWxy() const;
        QString getParameterWz() const;
        QString getParameterGamma() const;
        QString getParameterPixelSize() const;
        bool getN() const;
        bool getTriplet() const;
        bool getFocusParams() const;
    private:
        Ui::QFFCSMSDEvaluationGetNFromFits *ui;
};

#endif // QFFCSMSDEVALUATIONGETNFROMFITS_H
