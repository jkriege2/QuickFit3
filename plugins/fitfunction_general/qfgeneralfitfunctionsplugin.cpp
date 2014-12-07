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

#include "qfgeneralfitfunctionsplugin.h"
#include "qffitfunctiongeneralgaussianvar.h"
#include "qffitfunctiongeneralskewgaussianvar.h"
#include "qffitfunctiongeneral2gaussianvar.h"
#include "qffitfunctiongeneral3gaussianvar.h"
#include "qffitfunctiongeneralsigmoid.h"
#include "qffitfunctiongenerallorentzian.h"
#include "qffitfunctiongeneralexp.h"
#include "qffitfunctiongeneraldblexp.h"
#include "qffitfunctiongeneralpowerlaw.h"
#include "qffitfunctiongeneralsine.h"
#include "qffitfunctiongeneralcosine.h"
#include "qffitfunctiongeneralline.h"
#include "qffitfunctiongeneralpolynom.h"
#include "qffitfunctiongeneralpolynomcentered.h"
#include "qffitfunctiongenerallognormal.h"
#include "qffitfunctiongeneralmodhill5p.h"
#include "qffitfunctiongeneralhill.h"
#include "qffitfunctiongeneralgaussiandistvar.h"
#include "qffitfunctiongeneral2lognormal.h"
#include "qffitfunctiongeneralsegmentedline.h"
#include "qffitfunctiongeneralsegmentedpowerlaw.h"
#include "qffitfunctiongeneralparabula.h"
#include "qffitfunctiongeneralgaussianbeamwidth.h"
#include "qffitfunctiongeneralgaussianbeamzintensity.h"
#include "qffitfunctiongeneralpdfgaussianvar.h"
#include "qffitfunctiongeneralcdfgaussiandistvar.h"
#include "qffitfunctiongeneralpdfcauchy.h"
#include "qffitfunctiongeneralcdfcauchy.h"
#include "qffitfunctiongeneralpdfloggaussian.h"
#include "qffitfunctiongeneralcdfloggaussian.h"

QStringList QFGeneralFitFunctionsPlugin::getIDs() const {
    QStringList res;
    res<<"gen_gaussian_sqrte";
    res<<"gen_skewgaussian_sqrte";
    res<<"gen_gaussiandist_sqrte";
    res<<"gen_2gaussian_sqrte";
    res<<"gen_3gaussian_sqrte";
    res<<"gen_lorentzian";
    res<<"gen_lognormal";
    res<<"gen_2lognormal";
    res<<"gen_sine";
    res<<"gen_cos";
    res<<"gen_sigmoid";
    res<<"gen_exp";
    res<<"gen_dblexp";
    res<<"gen_powerlaw";
    res<<"gen_segpowerlaw";
    res<<"gen_line";
    res<<"gen_polynom";
    res<<"gen_parabula";
    res<<"gen_polynomcenter";
    res<<"gen_hill";
    res<<"gen_modhill5p";
    res<<"gen_segline";
    res<<"gen_gaussianbeam_width";
    res<<"gen_gaussianbeam_zintensity";
    res<<"gen_pdf_gaussian";
    res<<"gen_cdf_gaussian";
    res<<"gen_pdf_loggaussian";
    res<<"gen_cdf_loggaussian";
    res<<"gen_pdf_cauchy";
    res<<"gen_cdf_cauchy";
    return res;
}

QFFitFunction* QFGeneralFitFunctionsPlugin::get(QString id, QObject* parent) const  {
    if (id=="gen_sigmoid") {
        return new QFFitFunctionGeneralSigmoid();
    } else if (id=="gen_gaussian_sqrte") {
        return new QFFitFunctionGeneralGaussianVar();
    } else if (id=="gen_skewgaussian_sqrte") {
        return new QFFitFunctionGeneralSkewGaussianVar();
    } else if (id=="gen_gaussiandist_sqrte") {
        return new QFFitFunctionGeneralGaussianDistVar();
    } else if (id=="gen_2gaussian_sqrte") {
        return new QFFitFunctionGeneral2GaussianVar();
    } else if (id=="gen_3gaussian_sqrte") {
        return new QFFitFunctionGeneral3GaussianVar();
    } else if (id=="gen_lorentzian") {
        return new QFFitFunctionGeneralLorentzian();
    } else if (id=="gen_exp") {
        return new QFFitFunctionGeneralExp();
    } else if (id=="gen_dblexp") {
        return new QFFitFunctionGeneralDblExp();
    } else if (id=="gen_powerlaw") {
        return new QFFitFunctionGeneralPowerLaw();
    } else if (id=="gen_segpowerlaw") {
        return new QFFitFunctionGeneralSegmentedPowerLaw();
    } else if (id=="gen_sine") {
        return new QFFitFunctionGeneralSine();
    } else if (id=="gen_cos") {
        return new QFFitFunctionGeneralCosine();
    } else if (id=="gen_lognormal") {
        return new QFFitFunctionGeneralLogNormal();
    } else if (id=="gen_2lognormal") {
        return new QFFitFunctionGeneral2LogNormal();
    } else if (id=="gen_line") {
        return new QFFitFunctionGeneralLine();
    } else if (id=="gen_polynom") {
        return new QFFitFunctionGeneralPolynom();
    } else if (id=="gen_parabula") {
        return new QFFitFunctionGeneralParabula();
    } else if (id=="gen_polynomcenter") {
        return new QFFitFunctionGeneralPolynomCentered();
    } else if (id=="gen_hill") {
        return new QFFitFunctionGeneralHill();
    } else if (id=="gen_modhill5p") {
        return new QFFitFunctionGeneralModHill5P();
    } else if (id=="gen_segline") {
        return new QFFitFunctionGeneralSegmentedLine();
    } else if (id=="gen_gaussianbeam_width") {
        return new QFFitFunctionGeneralGaussianBeamWidth();
    } else if (id=="gen_gaussianbeam_zintensity") {
        return new QFFitFunctionGeneralGaussianBeamZIntensity();
    } else if (id=="gen_pdf_gaussian") {
        return new QFFitFunctionGeneralPDFGaussianVar();
    } else if (id=="gen_cdf_gaussian") {
        return new QFFitFunctionGeneralCDFGaussianDistVar();
    } else if (id=="gen_pdf_loggaussian") {
        return new QFFitFunctionGeneralPDFLogGaussian();
    } else if (id=="gen_cdf_loggaussian") {
        return new QFFitFunctionGeneralCDFLogGaussian();
    } else if (id=="gen_pdf_cauchy") {
        return new QFFitFunctionGeneralPDFCauchy();
    } else if (id=="gen_cdf_cauchy") {
        return new QFFitFunctionGeneralCDFCauchy();
    }
    return NULL;
}

Q_EXPORT_PLUGIN2(fitfunction_general, QFGeneralFitFunctionsPlugin)
