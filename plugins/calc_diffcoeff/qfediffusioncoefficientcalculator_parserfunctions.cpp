
/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2014-09-12 18:00:48 +0200 (Fr, 12 Sep 2014) $  (revision $Rev: 3461 $)

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

#include "qfediffusioncoefficientcalculator_parserfunctions.h"
#include "qfediffusioncoefficientcalculator.h"

void fCalcD_getViscosityWater(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    QString fname="calcd_getviscositywater";
    res.setInvalid();
    QFEDiffusionCoefficientCalculator* inst=QFEDiffusionCoefficientCalculator::getInstance();
    if (inst) {
        if (n==1 && params[0].type==qfmpDouble) {
            res.setDouble(inst->getSolutionViscosity(0, params[0].num));
        } else if (n==1 && params[0].type==qfmpDoubleVector) {
            res.setDoubleVec(params[0].numVec);
            for (int i=0; i<res.numVec.size(); i++) {
                res.numVec[i]=inst->getSolutionViscosity(0, res.numVec[i]);
            }
        } else {
            parser->qfmpError(QObject::tr("%1(temperature): requires one number parameters").arg(fname));
        }
    } else {
        parser->qfmpError(QObject::tr("%1: plugin 'calc_diffcoeff' not loaded").arg(fname));
    }
}

void fCalcD_getDSphere(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    QString fname="calcd_getdiffcoeffsphere";
    res.setInvalid();
    QFEDiffusionCoefficientCalculator* inst=QFEDiffusionCoefficientCalculator::getInstance();
    if (inst) {
        double T=293.15;
        if (n>=2 && params[1].type==qfmpDouble) {
            T=params[1].num;
        }

        if (n>=1 && params[0].type==qfmpDouble) {
            res.setDouble(inst->getSphereDCoeff(0, params[0].num, T));
        } else {
            parser->qfmpError(QObject::tr("%1(diameter, temperature=293.15): requires one or two number parameters").arg(fname));
        }
    } else {
        parser->qfmpError(QObject::tr("%1: plugin 'calc_diffcoeff' not loaded").arg(fname));
    }
}

void fCalcD_getDRotSphere(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    QString fname="calcd_getdiffcoeffrotsphere";
    res.setInvalid();
    QFEDiffusionCoefficientCalculator* inst=QFEDiffusionCoefficientCalculator::getInstance();
    if (inst) {
        double T=293.15;
        if (n>=2 && params[1].type==qfmpDouble) {
            T=params[1].num;
        }

        if (n>=1 && params[0].type==qfmpDouble) {
            res.setDouble(inst->getSphereDRotCoeff(0, params[0].num, T));
        } else {
            parser->qfmpError(QObject::tr("%1(diameter, temperature=293.15): requires one or two number parameters").arg(fname));
        }
    } else {
        parser->qfmpError(QObject::tr("%1: plugin 'calc_diffcoeff' not loaded").arg(fname));
    }
}
void fCalcD_getTauDRotSphere(qfmpResult &res, const qfmpResult *params, unsigned int n, QFMathParser *parser)
{
    QString fname="calcd_gettaudiffrotsphere";
    res.setInvalid();
    QFEDiffusionCoefficientCalculator* inst=QFEDiffusionCoefficientCalculator::getInstance();
    if (inst) {
        double T=293.15;
        if (n>=2 && params[1].type==qfmpDouble) {
            T=params[1].num;
        }

        if (n>=1 && params[0].type==qfmpDouble) {
            res.setDouble(1.0/6.0/inst->getSphereDRotCoeff(0, params[0].num, T));
        } else {
            parser->qfmpError(QObject::tr("%1(diameter, temperature=293.15): requires one or two number parameters").arg(fname));
        }
    } else {
        parser->qfmpError(QObject::tr("%1: plugin 'calc_diffcoeff' not loaded").arg(fname));
    }
}
