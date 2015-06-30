/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

    This file is part of QuickFit 3 (http://www.dkfz.de/Macromol/quickfit).

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License (LGPL) for more details.

    You should have received a copy of the GNU Lesser General Public License (LGPL)
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qfmathtools.h"
#include <float.h>
#include <QDebug>
#include "qftools.h"
#include <complex>
#include "Faddeeva.hh"
#include "statistics_tools.h"

double qfTanc(  double x ) {
    if (x==0) return 1;
    return tan(x)/x;
}


double qfSinc(  double x )
{
    static double const    taylor_0_bound = 3*DBL_MIN ;
    static double const    taylor_2_bound = sqrt(taylor_0_bound);
    static double const    taylor_n_bound = sqrt(taylor_2_bound);

    if    (fabs(x) >= taylor_n_bound)
    {
        return sin(x)/x;
    }
    else
    {
        // approximation by taylor series in x at 0 up to order 0
        double    result = 1;

        if    (fabs(x) >= taylor_0_bound)
        {
            double    x2 = x*x;

            // approximation by taylor series in x at 0 up to order 2
            result -= x2/static_cast<double>(6);

            if    (fabs(x) >= taylor_2_bound)
            {
                // approximation by taylor series in x at 0 up to order 4
                result += (x2*x2)/static_cast<double>(120);
            }
        }

        return result;
    }
}


double qfFaddeevaRealW( double xi) {
    std::complex<double> d(0,xi);
    std::complex<double> w=Faddeeva::w(d);
    return w.real();
}

template <class T>
double qf_statisticsAverage(const T* data, long long N) {
    if (N<=0) return 0;
    register double sum=0;
    for (register long long i=0; i<N; i++) {
        sum=sum+(double)data[i];
    }
    return sum/(double)N;
}

void qf_statisticsAutocorrelate(double* dataout, const double* data, long long N) {
    if (N<=0) return;
    register double avg=qf_statisticsAverage<double>(data, N);
    register long long kmax=ceil((double)N/2.0);
    for (register long long k=0; k<kmax; k++) {
        register double sum=0;
        for (register long long i=0; i<(N-k); i++) {
            sum=sum+(data[i]-avg)*(data[i+k]-avg);
        }
        dataout[k]=sum/(double)N;
    }
}

double* qfstatisticsAllocAutocorrelate(const double* data, long long N) {
    double* d=(double*)qfMalloc(ceil((double)N/2.0)*sizeof(double));
    qf_statisticsAutocorrelate(d, data, N);
    return d;
}

double roundError(double error, int addSignifcant) {
    if (fabs(error)<DBL_MIN*10.0) return error;
    int sbits_error=ceil(log(fabs(error))/log(10.0));
    double f=pow(10.0, sbits_error-1-addSignifcant);
    //qDebug()<<"roundError("<<error<<addSignifcant<<")  sbits_error="<<sbits_error<<"  f="<<f;
    return round(error/f)*f;
}

double roundWithError(double value, double error, int addSignifcant)  {
    if ((fabs(error)<DBL_MIN*10)||(!std::isfinite(error))) return value;

    int sbits_error=ceil(log(fabs(error))/log(10.0));
    if (sbits_error>=0) sbits_error=-1;
    double f=pow(10.0, sbits_error-(addSignifcant+1));

    return round(value/f)*f;
}


QFFitStatistics::QFFitStatistics():
    QFBasicFitStatistics()
{
    runAvgStart=0;
    maxRelParamError=0;
    AICc=0;
    AICcWeighted=0;
    BIC=0;
    BICweighted=0;
    RsquaredWeighted=0;
    AdjustedRsquaredWeighted=0;
    //fitfunc=0;
    //residuals=0;
    //residuals_weighted=0;
    runAvgMaxN=0;
    runAvgN=0;
    //tau_runavg=0;
    //residuals_runavg=0;
    //residuals_runavg_weighted=0;
    fitparamN=0;
    dataSize=0;
    degFreedom=0;
    residSqrSum=0;
    residWeightSqrSum=0;
    residSum=0;
    residWeightSum=0;
    gSum=0;
    gSqrSum=0;
    bayesProbability=NAN;
    bayesProbabilityLog10=NAN;


    rmin=0;
    rmax=0;
    rminw=0;
    rmaxw=0;
    residAverage=0;
    residWeightAverage=0;
    residStdDev=0;
    residWeightStdDev=0;
    TSS=0;
    Rsquared=0;
    AdjustedRsquared=0;

    residHistBinWidth=0;
    residHistWBinWidth=0;
    //resHistogram=0;
    resHistogramCount=0;
    //resWHistogram=0;
    resWHistogramCount=0;
    //resCorrelation=0;
    //resWCorrelation=0;
    resN=0;
}

void QFFitStatistics::free() {
    QFBasicFitStatistics::free();

    /*if (fitfunc) { std::free(fitfunc); fitfunc=NULL; }
    if (residuals) { std::free(residuals); residuals=NULL; }
    if (residuals_weighted) { std::free(residuals_weighted); residuals_weighted=NULL; }
    if (tau_runavg) { std::free(tau_runavg); tau_runavg=NULL; }
    if (residuals_runavg) { std::free(residuals_runavg); residuals_runavg=NULL; }
    if (residuals_runavg_weighted) { std::free(residuals_runavg_weighted); residuals_runavg_weighted=NULL; }
    if (resHistogram) { std::free(resHistogram); resHistogram=NULL; }
    if (resWHistogram) { std::free(resWHistogram); resWHistogram=NULL; }
    if (resCorrelation) { std::free(resCorrelation); resCorrelation=NULL; }
    if (resWCorrelation) {std::free(resWCorrelation); resWCorrelation=NULL; }*/
}

QString QFFitStatistics::getHTMLExplanation() const
{
    return QFBasicFitStatistics::getHTMLExplanation();
}

QString QFFitStatistics::getAsHTMLTable(bool addExplanation, bool includeR2) const
{
    return QFBasicFitStatistics::getAsHTMLTable(addExplanation, includeR2);
}




QFFitStatistics calculateFitStatistics(long N, const double* tauvals, const double* model, const double* corrdata, const double* weights, int datacut_min, int datacut_max, int paramCount, int runAvgWidth, int residualHistogramBins, const double* fitFuncParams, const double* fitFuncParamErrors, const QVector<double>& COV, double paramrange_size) {
    datacut_max=qBound((long)datacut_min, (long)datacut_max, (long)N-1);
    QFFitStatistics result;


    /////////////////////////////////////////////////////////////////////////////////
    // retrieve data and tau-values from rawdata record
    /////////////////////////////////////////////////////////////////////////////////
    result.runAvgStart=-1*runAvgWidth/2;
    result.fitfunc.resize(N);//=(double*)qfCalloc(N,sizeof(double));
    result.residuals.resize(N);//=(double*)qfCalloc(N,sizeof(double));
    result.residuals_weighted.resize(N);//=(double*)qfCalloc(N,sizeof(double));
    result.runAvgMaxN=N;
    result.runAvgN=0;
    result.tau_runavg.resize(result.runAvgMaxN);//=(double*)qfCalloc(result.runAvgMaxN, sizeof(double));
    result.residuals_runavg.resize(result.runAvgMaxN);//=(double*)qfCalloc(result.runAvgMaxN, sizeof(double));
    result.residuals_runavg_weighted.resize(result.runAvgMaxN);//=(double*)qfCalloc(result.runAvgMaxN, sizeof(double));

    /////////////////////////////////////////////////////////////////////////////////
    // retrieve fit parameters and errors. run calcParameters to fill in calculated parameters and make sure
    // we are working with a complete set of parameters
    /////////////////////////////////////////////////////////////////////////////////
    result.fitparamN=paramCount;
    result.dataSize=datacut_max-datacut_min;
    result.degFreedom=result.dataSize-result.fitparamN-1;

    double detCOV=NAN;
    double COVSqrtDiagProd=NAN;
    //double COVDiagProd=NAN;
    if (COV.size()>0) {
        COVSqrtDiagProd=1.0;
        //COVDiagProd=1.0;
        int n=floor(sqrt(COV.size()));
        detCOV=linalgMatrixDeterminant(COV.constData(), n);
        QVector<double> sigmas;
        for (int i=0; i<n; i++) {
            COVSqrtDiagProd=COVSqrtDiagProd*sqrt(COV[i*n+i]);
            //COVDiagProd=COVDiagProd*COV[i*n+i];
            sigmas<<sqrt(COV[i*n+i]);
        }
        //qDebug()<<"COV=\n"<<linalgMatrixToString(COV.data(), n,n).c_str();
        //qDebug()<<"SIGMAS="<<sigmas;
        //qDebug()<<"  detCOV="<<detCOV<<"  COVSqrtDiagProd="<<COVSqrtDiagProd<</*"   COVDiagProd="<<COVDiagProd<<*/"  n="<<n<<"  paramCount="<<paramCount;
    }
    result.detCOV=detCOV;


    /////////////////////////////////////////////////////////////////////////////////
    // calculate model function values, residuals and residual parameters/statistics
    /////////////////////////////////////////////////////////////////////////////////
    result.residSqrSum=0;        // sum of squared residuals
    result.residWeightSqrSum=0;  // sum of squared weighted residuals
    result.residSum=0;           // sum of residuals
    result.residWeightSum=0;     // sum of weightedresiduals
    result.gSum=0;               // sum of measured values
    result.gSqrSum=0;            // sum of squared measured values


    result.rmin=0;       // min of residuals
    result.rmax=0;       // max of residuals
    result.rminw=0;      // min of weighted residuals
    result.rmaxw=0;      // max of weighted residuals
    result.weightSum=0; // weight sum (for normalization)
    bool hfirst=true;
    bool hasWeights=(weights!=NULL);
    bool allWeightsOne=true;
    bool allWeightsEqual=true;
    //double prodsigma=1.0;
    //double sumlogsigma=0;
    if (!weights) {
        //prodsigma=pow(1.0/double(datacut_max-datacut_min), datacut_max-datacut_min);
        //sumlogsigma=log(1.0/double(datacut_max-datacut_min))*double(datacut_max-datacut_min);
        result.weightSum=1;
    } else {
        for (int i=0; i<N; i++) {
            if ((i>=datacut_min)&&(i<=datacut_max)) {
                result.weightSum=result.weightSum+weights[i];
                allWeightsOne=allWeightsOne&&(weights[i]==1.0);
                allWeightsEqual=allWeightsEqual&&(weights[i]==weights[datacut_min]);
            }
        }
    }

    for (int i=0; i<N; i++) {
        double value=model[i];
        result.fitfunc[i]=value;
        result.residuals[i]=corrdata[i]-value;
        double res=result.residuals[i];
        if (weights) {
            result.residuals_weighted[i]=res/weights[i];

            if (fabs(weights[i])<1000.0*DBL_MIN) {
                result.residuals_weighted[i]=0;
            }

        } else {
            result.residuals_weighted[i]=res;
        }
        //std::cout<<"weights["<<i<<"]="<<weights[i]<<"\n";
        double resw=result.residuals_weighted[i];
        if ((i>=datacut_min)&&(i<=datacut_max)) {


            result.residSqrSum+=res*res;
            result.residWeightSqrSum+=resw*resw;
            result.residSum+=res;
            result.residWeightSum+=resw;
            result.gSum+=corrdata[i];
            result.gSqrSum+=corrdata[i]*corrdata[i];

            if (hfirst) {
                result.rmin=res;
                result.rmax=res;
                result.rmaxw=resw;
                result.rminw=resw;
                hfirst=false;
            } else {
                if (res>result.rmax) result.rmax=res;
                if (res<result.rmin) result.rmin=res;
                if (resw>result.rmaxw) result.rmaxw=resw;
                if (resw<result.rminw) result.rminw=resw;
            }
            //std::cout<<"res="<<res<<" resw="<<resw<<"    :    rmin="<<rmin<<"  rmax="<<rmax<<"    rminw="<<rminw<<"  rmaxw="<<rmaxw<<std::endl;
            if ((i+result.runAvgStart>=datacut_min) && (i+result.runAvgStart+runAvgWidth<=datacut_max) /*&& ((i-datacut_min)%runAvgWidth==0)*/) {
                double s=0, sw=0;
                double tau=0;
                for (int j=0; j<runAvgWidth; j++) {
                    int idx=i+j+result.runAvgStart;
                    s=s+result.residuals[idx];
                    sw=sw+result.residuals_weighted[idx];
                    tau+=tauvals[idx];
                }
                result.tau_runavg[result.runAvgN]=tauvals[i];
                result.residuals_runavg[result.runAvgN]=s/(double)runAvgWidth;
                result.residuals_runavg_weighted[result.runAvgN]=sw/(double)runAvgWidth;
                result.runAvgN++;
            }
        }
    }


    result.residAverage=result.residSum/(double)result.dataSize;
    result.residWeightAverage=result.residWeightSum/(double)result.dataSize;
    result.residStdDev=sqrt(result.residSqrSum/(double)result.dataSize-result.residSum*result.residSum/(double)result.dataSize/(double)result.dataSize);
    result.residWeightStdDev=sqrt(result.residWeightSqrSum/(double)result.dataSize-result.residWeightSum*result.residWeightSum/(double)result.dataSize/(double)result.dataSize);
    result.TSS=result.gSqrSum-result.gSum*result.gSum/(double)result.dataSize;
    result.Rsquared=1.0-result.residSqrSum/result.TSS;
    result.AdjustedRsquared=1.0-result.residSqrSum/(double(result.dataSize-result.fitparamN))/(result.TSS/(double(result.dataSize-1)));

    result.RsquaredWeighted=1.0-result.residWeightSqrSum/result.TSS;
    result.AdjustedRsquaredWeighted=1.0-result.residWeightSqrSum/(double(result.dataSize-result.fitparamN))/(result.TSS/(double(result.dataSize-1)));

    {
        const double p=double(result.fitparamN);
        const double n=double(result.dataSize);
        result.AICc=n*log(result.residSqrSum/n)+2.0*p+2.0*p*(p+1)/(n-p-1.0);
        result.AICcWeighted=n*log(result.residWeightSqrSum/n)+2.0*p+2.0*p*(p+1)/(n-p-1.0);
        result.BIC=n*log(result.residSqrSum/n)+p*log(n);
        result.BICweighted=n*log(result.residWeightSqrSum/n)+p*log(n);
    }

    result.residHistBinWidth=(result.rmax-result.rmin)/(double)residualHistogramBins;
    result.residHistWBinWidth=(result.rmaxw-result.rminw)/(double)residualHistogramBins;
    //if (QFFloatIsOK(detCOV)) result.detCOV=detCOV;
    if (QFFloatIsOK(paramrange_size)) result.bayesProbabilityParamRangeSize=paramrange_size;

    if (QFFloatIsOK(paramrange_size) && QFFloatIsOK(detCOV)){
        const double k=double(result.fitparamN);
        const double n=double(result.dataSize);
        const double chi2=result.residWeightSqrSum;
        const double ln2pi=log(2.0*M_PI);

        result.bayesProbabilityLog10= (0.5*k*ln2pi + 0.5*log(detCOV) - 0.5*chi2 - log(pow(2.0*paramrange_size, k)*COVSqrtDiagProd))/log(10.0) ;

        result.bayesProbability=pow(10.0, result.bayesProbabilityLog10 );

        //qDebug()<<"  calc Bayes prob: k="<<k<<" n="<<n<<" chi2="<<chi2<<" ln2pi="<<ln2pi<<" detCOV="<<detCOV<<" ln(detCOV)="<<log(detCOV)<<" COVSqrtDiagProd="<<COVSqrtDiagProd<<" paramrange_size="<<paramrange_size;
        //qDebug()<<"    -> "<<(0.5*k*ln2pi) << (0.5*log(detCOV)) << (- 0.5*chi2 ) << (- log(pow(2.0*paramrange_size, k)*COVSqrtDiagProd));
        //qDebug()<<"    => "<<result.bayesProbability<<"  = 10^"<<result.bayesProbabilityLog10;
    } else {
        //qDebug()<<"  nocalcBayes paramrange_size="<<paramrange_size<<" detCOV="<<detCOV;
    }


    /////////////////////////////////////////////////////////////////////////////////
    // calculate residual histogram
    /////////////////////////////////////////////////////////////////////////////////
    result.resHistogram.resize(residualHistogramBins);//=(double*)qfCalloc(residualHistogramBins, sizeof(double));
    result.resHistogramCount=0;
    result.resWHistogram.resize(residualHistogramBins);//=(double*)qfCalloc(residualHistogramBins, sizeof(double));
    result.resWHistogramCount=0;
    for (int i=0; i<N; i++) {
        if ((i>=datacut_min)&&(i<=datacut_max)) {
            if (result.residHistBinWidth>0) {
                int idx=round((result.residuals[i]-result.rmin)/result.residHistBinWidth);
                if ((idx>=0) && (idx<residualHistogramBins)) {
                    result.resHistogramCount++;
                    result.resHistogram[idx]++;
                }
            }
            if (result.residHistWBinWidth>0) {
                int idx=round((result.residuals_weighted[i]-result.rminw)/result.residHistWBinWidth);
                if ((idx>=0) && (idx<residualHistogramBins)) {
                    result.resWHistogramCount++;
                    result.resWHistogram[idx]++;
                }
            }
        }
    }
    if (result.resHistogramCount>0) for (int i=0; i<residualHistogramBins; i++) {
        result.resHistogram[i]=result.resHistogram[i]/result.resHistogramCount;
    }
    if (result.resWHistogramCount>0) for (int i=0; i<residualHistogramBins; i++) {
        result.resWHistogram[i]=result.resWHistogram[i]/result.resWHistogramCount;
    }



    /////////////////////////////////////////////////////////////////////////////////
    // calculate residual correlation
    /////////////////////////////////////////////////////////////////////////////////
    result.resN=ceil((double)(datacut_max-datacut_min)/2.0);
    result.resCorrelation.resize(result.resN+1);//=qfstatisticsAllocAutocorrelate(&(result.residuals[datacut_min]), datacut_max-datacut_min);
    result.resWCorrelation.resize(result.resN+1);//=qfstatisticsAllocAutocorrelate(&(result.residuals_weighted[datacut_min]), datacut_max-datacut_min);
    double* dat=result.residuals.data();
    qf_statisticsAutocorrelate(result.resCorrelation.data(), &(dat[datacut_min]), datacut_max-datacut_min);
    dat=result.residuals_weighted.data();
    qf_statisticsAutocorrelate(result.resWCorrelation.data(), &(dat[datacut_min]), datacut_max-datacut_min);
    for (register int i=0; i<result.resN; i++) {
        result.resCorrelation[i]/=(result.residStdDev*result.residStdDev);
        result.resWCorrelation[i]/=(result.residWeightStdDev*result.residWeightStdDev);
    }

    if (fitFuncParams && fitFuncParamErrors && paramCount>0) {
        result.maxRelParamError=0;
        for (int i=0; i<paramCount; i++) {
            double re=fitFuncParamErrors[i]/fabs(fitFuncParams[i]);
            if (re<1e2 || fabs(fitFuncParams[i])>1e18) {
                result.maxRelParamError=qMax(result.maxRelParamError, re);
            }
        }
    }

    return result;
}






QFBasicFitStatistics calculateBasicFitStatistics(long N, const double* tauvals, const double* model, const double* corrdata, const double* weights, int datacut_min, int datacut_max, int paramCount, const double* fitFuncParams, const double* fitFuncParamErrors, const QVector<double>& COV, double paramrange_size) {
    QFFitStatistics result=calculateFitStatistics(N, tauvals, model, corrdata, weights, datacut_min, datacut_max, paramCount, 11, 11, fitFuncParams, fitFuncParamErrors, COV, paramrange_size);
    return result;
}



QFBasicFitStatistics::QFBasicFitStatistics()
{
    maxRelParamError=0;
    AICc=0;
    AICcWeighted=0;
    BIC=0;
    BICweighted=0;
    RsquaredWeighted=0;
    AdjustedRsquaredWeighted=0;
    fitparamN=0;
    dataSize=0;
    degFreedom=0;
    residSqrSum=0;
    residWeightSqrSum=0;
    residSum=0;
    residWeightSum=0;
    gSum=0;
    gSqrSum=0;
    bayesProbability=NAN;
    bayesProbabilityParamRangeSize=NAN;
    detCOV=NAN;


    rmin=0;
    rmax=0;
    rminw=0;
    rmaxw=0;
    residAverage=0;
    residWeightAverage=0;
    residStdDev=0;
    residWeightStdDev=0;
    TSS=0;
    Rsquared=0;
    AdjustedRsquared=0;

}

void QFBasicFitStatistics::free()
{

}

QString QFBasicFitStatistics::getHTMLExplanation() const
{
    return QString("<font size=\"-1\"><i>Legend:</i>: &chi;<sup>2</sup>: sum error square, &lang;E&rang;: residual average, &radic;&lang;E2&rang;: residual stddev., det(COV): determinant of fit var-covariance matrix,<br>NP: number of fit parameters, NR: number of residuals, <br>DF: degrees of freedom, R<sup>2</sup>: coefficient of determination, <br>R<sup>2</sup><sub>adjusted</sub>: adjusted coefficient of determination, TSS: total sum of squares, max<sub>P</sub>(&sigma;<sub>P</sub>/|P|): maximum relative parameter error<br>AICc: Akaike's infromation criterion, BIC: Bayes information criterion, p<sub>Bayes</sub>(model|data): Bayes model probability</font>");
}

QString QFBasicFitStatistics::getAsHTMLTable(bool addExplanation, bool includeR2) const
{
    QString txtFit;
    txtFit+=QString("<table border=\"0\" width=\"95%\">");
    //txtFit+=QString("<tr><td align=\"right\"></td><td align=\"left\"></td><td align=\"right\"></td><td align=\"left\"></td></tr>");
    txtFit+=QString("<tr>"
                    "<td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                    "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                    "<td align=\"right\" valign=\"bottom\"><font size=\"+2\">&chi;<sup>2</sup></font> (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                    "</tr>").arg(residSqrSum).arg(residWeightSqrSum);
    txtFit+=QString("<tr>"
                    "<td align=\"right\" valign=\"bottom\">&lang;E&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                    "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                    "<td align=\"right\" valign=\"bottom\"> &lang;E&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                    "</tr>").arg(residAverage).arg(residWeightAverage);
    txtFit+=QString("<tr>"
                    "<td align=\"right\" valign=\"bottom\">&radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang;=</td><td align=\"left\" valign=\"bottom\">%1</td>"
                    "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                    "<td align=\"right\" valign=\"bottom\"> &radic;&lang;E<sup><font size=\"+1\">2</font></sup>&rang; (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                    "</tr>").arg(residStdDev).arg(residWeightStdDev);
    txtFit+=QString("<tr>"
                    "<td align=\"right\" valign=\"bottom\">NP =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                    "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                    "<td align=\"right\" valign=\"bottom\">NR =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                    "</tr>").arg(fitparamN).arg(dataSize);
    txtFit+=QString("<tr>"
                    "<td align=\"right\" valign=\"bottom\">DF =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                    "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                    "<td align=\"right\" valign=\"bottom\">TSS =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                    "</tr>").arg(degFreedom).arg(TSS);
    if (includeR2){
        txtFit+=QString("<tr>"
                        "<td align=\"right\" valign=\"bottom\">R<sup>2</sup> =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                        "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                        "<td align=\"right\" valign=\"bottom\">R<sup>2</sup> (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                        "</tr>").arg(Rsquared).arg(RsquaredWeighted);
        txtFit+=QString("<tr>"
                        "<td align=\"right\" valign=\"bottom\">R<sup>2</sup><sub>adjusted</sub> =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                        "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                        "<td align=\"right\" valign=\"bottom\">R<sup>2</sup><sub>adjusted</sub> (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                        "</tr>").arg(AdjustedRsquared).arg(AdjustedRsquaredWeighted);
    }
    txtFit+=QString("<tr>"
                    "<td align=\"right\" valign=\"bottom\">AICc =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                    "<td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>"
                    "<td align=\"right\" valign=\"bottom\">AICc (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                    "</tr>").arg(AICc).arg(AICcWeighted);
    txtFit+=QString("<tr>"
                    "<td align=\"right\" valign=\"bottom\">BIC =</td><td align=\"left\" valign=\"bottom\">%1</td>"
                    "<td></td>"
                    "<td align=\"right\" valign=\"bottom\">BIC (weighted) =</td><td align=\"left\" valign=\"bottom\">%2</td>"
                    "</tr>").arg(BIC).arg(BICweighted);


    if (maxRelParamError>0.0/* && (bayesProbability<0.0 || !QFFloatIsOK(bayesProbability))*/) {
        txtFit+=QString("<tr>"
                        "<td align=\"right\" valign=\"bottom\"></td><td align=\"left\" valign=\"bottom\"></td>"
                        "<td></td>"
                        "<td align=\"right\" valign=\"bottom\">max<sub>P</sub>(&sigma;<sub>P</sub>/|P|) =</td><td align=\"left\" valign=\"bottom\">%1%2</td>"
                        "</tr>").arg(maxRelParamError*100.0).arg("%");
    }

    QString btxt="";
    if (QFFloatIsOK(detCOV)) {
        btxt+=QString("<td align=\"right\" valign=\"bottom\">det(COV) =</td><td align=\"left\" valign=\"bottom\">%1</td>")
                .arg(detCOV);
    }
    if (QFFloatIsOK(detCOV) && (bayesProbability>=0.0 || bayesProbabilityLog10>=0)) {
        btxt+=QString("<td></td>");
    } else if (QFFloatIsOK(detCOV)) {
        btxt+=QString("<td></td><td></td><td></td>");
    }
    if (bayesProbability>=0.0 || bayesProbabilityLog10>=0) {
        btxt+=QString("<td align=\"right\" valign=\"bottom\">p<sub>Bayes</sub>(model|data) =</td><td align=\"left\" valign=\"bottom\">%1 ( = 10<sup><font size=\"+2\">%2</font></sup>)</td>")
                .arg(bayesProbability).arg(bayesProbabilityLog10);
    }
    if (btxt.size()>0) {
        txtFit+=QString("<tr>%1</tr>").arg(btxt);
    }
    txtFit+=QString("</table>");

    if (addExplanation) {
        txtFit.append(QString("<br><br>")+getHTMLExplanation());
    }

    //qDebug()<<maxRelParamError<<bayesProbability<<"\n"<<txtFit<<"\n\n\n";
    return txtFit;
}
