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
#include <QtGlobal>
#include "fcs_fitfuctions.h"


#include "qffitfunctionfcsmultidiffusion.h"
#include "qffitfunctionfcsadiff.h"
#include "qffitfunctionfcsdiff.h"
#include "qffitfunctionfcsadiff2d.h"
#include "qffitfunctionfcsdiff2d.h"
#include "qffitfunctionfcsdiff_relbright.h"
#include "qffitfunctionfcsadiff_relbright.h"
#include "qffitfunctionfcsexp.h"
#include "qffitfunctionfcssimplediff.h"
#include "qffitfunctionfcsflow.h"
#include "qffitfunctionfcsrot.h"
#include "qffitfunctionfcsoscillation.h"
#include "qffitfunctionfcsdiffD.h"
#include "qffitfunctionfcsflowv.h"
#include "qffitfunctioncsfcs.h"
#include "qffitfunctionfcspowerlaw.h"
#include "qffitfunctionfcssimpleexp.h"
#include "qffitfunctionfcsdiffDAfterpulse.h"
#include "qffitfunctionfcsdiffDcAfterpulse.h"
#include "qffitfunctionfcsdiff2d_relbright.h"
#include "qffitfunctionfcsadiff2d_relbright.h"
#include "qffitfunctionfcsdiffstretchpp.h"
#include "qffitfunctionfcsdiff2dstretchpp.h"






QFPFitFunctionFCS::QFPFitFunctionFCS(QObject *parent):
    QObject(parent)
{

}

QStringList QFPFitFunctionFCS::getIDs() const {
    QStringList res;
    res<<"fcs_diff"<<"fcs_diff2d"<<"fcs_tir_diff2d"
       <<"fcs_adiff_diffbright"<<"fcs_diff_diffbright"
      <<"fcs_diff2d_diffbright"<<"fcs_tir_diff2d_diffbright"
     <<"fcs_diff_d"<<"fcs_diff_d_afterpulse"
    <<"fcs_diff_d_c_afterpulse"<<"fcs_diff_d_wz"
    <<"fcs_diff_d_wz_afterpulse"<<"fcs_diff_d_wz_c_afterpulse"
    <<"fcs_diff1"<<"fcs_adiff"
    <<"fcs_multidiff"<<"fcs_diffflow"
    <<"fcs_diffflowv"<<"fcs_diffrot"
    <<"fcs_oscillation"<<"fcs_cs_diff_d"<<"fcs_diff_cs_d_wz"
    <<"fcs_simpleexp"<<"fcs_exp"<<"fcs_powerlaw"<<"fcs_diff_stretchpp"
    <<"fcs_diff2d_stretchpp"<<"fcs_adiff2d"<<"fcs_adiff2d_diffbright"
    <<"fcs_tir_diff2d_stretchpp"<<"fcs_tir_adiff2d"<<"fcs_tir_adiff2d_diffbright";
    return res;
}

QFFitFunction* QFPFitFunctionFCS::get(const QString &id) const  {
    if (id=="fcs_diff") {
        return new QFFitFunctionFCSDiff();
    } else if (id=="fcs_diff_stretchpp") {
        return new QFFitFunctionFCSDiffStretchPP();
    } else if (id=="fcs_diff2d_stretchpp") {
        return new QFFitFunctionFCSDiff2DStretchPP(false);
    } else if (id=="fcs_diff2d") {
        return new QFFitFunctionFCSDiff2D(false);
    } else if (id=="fcs_adiff2d") {
        return new QFFitFunctionFCSADiff2D(false);
    } else if (id=="fcs_tir_diff2d_stretchpp") {
        return new QFFitFunctionFCSDiff2DStretchPP(true);
    } else if (id=="fcs_tir_diff2d") {
        return new QFFitFunctionFCSDiff2D(true);
    } else if (id=="fcs_tir_adiff2d") {
        return new QFFitFunctionFCSADiff2D(true);
    } else if (id=="fcs_diff_d") {
        return new QFFitFunctionFCSDiffD(true);
    } else if (id=="fcs_exp") {
        return new QFFitFunctionFCSExp();
    } else if (id=="fcs_diff_d_wz") {
        return new QFFitFunctionFCSDiffD(false);
    } else if (id=="fcs_diff1") {
        return new QFFitFunctionFCSSimpleDiff();
    } else if (id=="fcs_adiff") {
        return new QFFitFunctionFCSADiff();
    } else if (id=="fcs_multidiff") {
        return new QFFitFunctionFCSMultiDiffusion();
    } else if (id=="fcs_diffflow") {
        return new QFFitFunctionFCSFlow();
    } else if (id=="fcs_diffrot") {
        return new QFFitFunctionFCSRot();
    } else if (id=="fcs_oscillation") {
        return new QFFitFunctionFCSOscillation();
    } else if (id=="fcs_diffflowv") {
        return new QFFitFunctionFCSFlowV();
    } else if (id=="fcs_cs_diff_d") {
        return new QFFitFunctionCSFCSDiffD(true);
    } else if (id=="fcs_diff_cs_d_wz") {
        return new QFFitFunctionCSFCSDiffD(false);
    } else if (id=="fcs_powerlaw") {
        return new QFFitFunctionFCSPowerLaw();
    } else if (id=="fcs_simpleexp") {
        return new QFFitFunctionFCSSimpleExp();
    } else if (id=="fcs_diff_d_afterpulse") {
        return new QFFitFunctionFCSDiffDAfterPulse(true);
    } else if (id=="fcs_diff_d_wz_afterpulse") {
        return new QFFitFunctionFCSDiffDAfterPulse(false);
    } else if (id=="fcs_diff_d_wz_c_afterpulse") {
        return new QFFitFunctionFCSDiffDcAfterPulse(false);
    } else if (id=="fcs_diff_d_c_afterpulse") {
        return new QFFitFunctionFCSDiffDcAfterPulse(true);
    } else if (id=="fcs_diff_diffbright") {
        return new QFFitFunctionFCSDiffDifferentBrightness();
    } else if (id=="fcs_diff2d_diffbright") {
        return new QFFitFunctionFCSDiff2DDifferentBrightness(false);
    } else if (id=="fcs_tir_diff2d_diffbright") {
        return new QFFitFunctionFCSDiff2DDifferentBrightness(true);
    } else if (id=="fcs_adiff_diffbright") {
        return new QFFitFunctionFCSADiffDifferentBrightness();
    } else if (id=="fcs_adiff2d_diffbright") {
        return new QFFitFunctionFCSADiff2DDifferentBrightness(false);
    } else if (id=="fcs_tir_adiff2d_diffbright") {
        return new QFFitFunctionFCSADiff2DDifferentBrightness(true);
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(qffcsfitfuncs, QFPFitFunctionFCS)

