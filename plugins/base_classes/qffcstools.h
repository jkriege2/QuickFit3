/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    

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

#ifndef QFFCSTOOLS_H
#define QFFCSTOOLS_H
#include "qfrawdatarecord.h"
#include "qfevaluationitem.h"
#include <QString>

bool qfFCSHasSpecial(const QFRawDataRecord *r, int index, const QString &paramid, double &value, double &error) ;

double qfFCSTripletTerm(double tau, double thetaT, double tauT);
double qfFCSTripletBlinkTerm(double tau, double thetaT, double tauT, double thetaT2, double tauT2);

bool qfFCSOverrideFitFunctionPreset(const QFEvaluationItem* eval, const QFRawDataRecord* r, QString paramid, double &value, bool checkPositive=false);
bool qfFCSOverrideFitFunctionPresetError(const QFEvaluationItem *eval, const QFRawDataRecord* r, QString paramid, double &value, bool checkPositive=false);
bool qfFCSOverrideFitFunctionPresetFix(const QFEvaluationItem* eval, const QFRawDataRecord* r, QString paramid, bool &value);

/** \brief correct FCCS crosstalk, according to see Bacia, Petrasek, Schwille, "Correcting Spectral Cross-Talk in Dual-Color FCCS", DOI: 10.1002/cphc.201100801 */
void qfFCCSCrosstalkCorrection(double& acf0, double& acf1, double& ccf, double I0, double I1, double crosstalk, int crosstalk_direction=0);
#endif // QFFCSTOOLS_H
