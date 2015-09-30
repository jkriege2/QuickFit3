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

#include<QDebug>
#include "analysis.h"
#include "gsl/gsl_randist.h"
//#define MYDEBUG



int analysisFRET(QVector<BurstDual> &burstVectorDual, const FRETparams_t &FRETparams)
{
    qDebug("analysisFRET");
    if (burstVectorDual.isEmpty()) {AlexEvalLog::warning("burst vector is empty");return EXIT_FAILURE;}


//    correction factors

    double crosstalk=FRETparams.crosstalk;
    double directExc = FRETparams.directExc;
    double f_Dexc, n_Dexc_raw, f_Dexc_gamma;
    double f_Dem_Dex, f_Aem_Dex, f_Dem_Aex, f_Aem_Aex; // background corrected photon counts
    double f_Aexc,n_Aexc_raw;
    BurstDual* burst;

//    qDebug()<<"crosstalk="<<crosstalk;
    for(int idx=0;idx<burstVectorDual.size();++idx) {
        burst=&burstVectorDual[idx]; // seems to work (?)
        n_Dexc_raw = burst->n_Dem_Dex + burst->n_Aem_Dex;
        n_Aexc_raw = burst->n_Dem_Aex + burst->n_Aem_Aex;


/* first approximation: each excitation period lasts half the burst duration
 * FIXME: not correct for different excitation lenghts (but correct if the sample used to measure the background had the same excitation periods
 *    double t_Dex=ad->getExcitationPeriodDonor();
 *    double t_Aex=ad->getExcitationPeriodAcceptor();
 */
/*  NOT corrected for intesities below zero
 *
        f_Dem_Dex=fmax(0, (double)burst->n_Dem_Dex - FRETparams.bg_Dem_Dex*burst->duration*FRETparams.dutyCycleDex);
        f_Aem_Dex=fmax(0, (double)burst->n_Aem_Dex - FRETparams.bg_Aem_Dex*burst->duration*FRETparams.dutyCycleDex);
        f_Dem_Aex=fmax(0, (double)burst->n_Dem_Aex - FRETparams.bg_Dem_Aex*burst->duration*FRETparams.dutyCycleAex);
        f_Aem_Aex=fmax(0, (double)burst->n_Aem_Aex - FRETparams.bg_Aem_Aex*burst->duration*FRETparams.dutyCycleAex);

        // scale acceptor exc
        f_Dem_Aex *= FRETparams.scaleAcceptor; // is neglegible/sometimes negative
        f_Aem_Aex *= FRETparams.scaleAcceptor;

//        f_Dexc = fmax(0, f_Dem_Dex + (f_Aem_Dex-crosstalk*burst->n_Dem_Dex-directExc*burst->n_Aem_Aex));
        f_Aem_Dex = (f_Aem_Dex - crosstalk*f_Dem_Dex - directExc*f_Aem_Aex);
        f_Dexc = ( f_Dem_Dex + f_Aem_Dex);
        f_Aexc = f_Dem_Aex + f_Aem_Aex;

        // FIXME: changed burst->n_Dem_Dex to f_Dem_Dex, so factors are evaluated with substracted background now!
        f_Dexc_gamma = fmax(0, FRETparams.gamma*f_Dem_Dex + f_Aem_Dex);
//        f_Dexc_gamma = FRETparams.gamma*f_Dem_Dex + (f_Aem_Dex - crosstalk*burst->n_Dem_Dex - directExc*burst->n_Aem_Aex);


//        burst->proximityRatio = (f_Aem_Dex - crosstalk*burst->n_Dem_Dex - directExc*burst->n_Aem_Aex)/(f_Dexc_gamma);
        burst->proximityRatio = f_Aem_Dex/(f_Dexc_gamma);
        burst->stoichiometryRatio = f_Dexc_gamma/(f_Dexc_gamma + f_Aexc);
*/

        if(FRETparams.useNegativeIntesities) {
            f_Dem_Dex= (double)burst->n_Dem_Dex - FRETparams.bg_Dem_Dex*burst->duration*FRETparams.dutyCycleDex;
            f_Aem_Dex= (double)burst->n_Aem_Dex - FRETparams.bg_Aem_Dex*burst->duration*FRETparams.dutyCycleDex;
            f_Dem_Aex= (double)burst->n_Dem_Aex - FRETparams.bg_Dem_Aex*burst->duration*FRETparams.dutyCycleAex;
            f_Aem_Aex=(double)burst->n_Aem_Aex - FRETparams.bg_Aem_Aex*burst->duration*FRETparams.dutyCycleAex;

            f_Aem_Dex =  f_Aem_Dex - crosstalk*f_Dem_Dex - directExc*f_Aem_Aex;
            f_Dexc =  f_Dem_Dex + f_Aem_Dex;
            f_Dexc_gamma =  FRETparams.gamma*f_Dem_Dex + f_Aem_Dex;
            f_Aexc = f_Dem_Aex + f_Aem_Aex;

        } else {
            f_Dem_Dex=fmax(0, (double)burst->n_Dem_Dex - FRETparams.bg_Dem_Dex*burst->duration*FRETparams.dutyCycleDex);
            f_Aem_Dex=fmax(0, (double)burst->n_Aem_Dex - FRETparams.bg_Aem_Dex*burst->duration*FRETparams.dutyCycleDex);
            f_Dem_Aex=fmax(0, (double)burst->n_Dem_Aex - FRETparams.bg_Dem_Aex*burst->duration*FRETparams.dutyCycleAex);
            f_Aem_Aex=fmax(0, (double)burst->n_Aem_Aex - FRETparams.bg_Aem_Aex*burst->duration*FRETparams.dutyCycleAex);

            f_Aem_Dex = fmax(0, f_Aem_Dex - crosstalk*f_Dem_Dex - directExc*f_Aem_Aex);
            f_Dexc = fmax(0, f_Dem_Dex + f_Aem_Dex);
            f_Dexc_gamma = fmax(0, FRETparams.gamma*f_Dem_Dex + f_Aem_Dex);
            f_Aexc = f_Dem_Aex + f_Aem_Aex;
        }

        if(f_Dexc<FRETparams.directExcThresholdDex) {
            burst->proximityRatio=1;
//            burst->stoichiometryRatio=0;
        } else {
            burst->proximityRatio = f_Aem_Dex/(f_Dexc_gamma);
        }
        burst->stoichiometryRatio = f_Dexc_gamma/(f_Dexc_gamma + f_Aexc);



// qDebug("f_Dexc=%f f_Aexc=%f",f_Dexc,f_Aexc);

        burst->proximityRatioRaw=(double)burst->n_Aem_Dex/n_Dexc_raw;

        burst->stoichiometryRatioRaw=n_Dexc_raw/(n_Dexc_raw+n_Aexc_raw);

        burst->stoichiometryRatioIntensity = f_Dexc_gamma/(f_Dexc_gamma + f_Aexc*FRETparams.dutyCycleDex/FRETparams.dutyCycleAex);

        burst->stoichiometryFraction = f_Dexc_gamma/f_Aexc;

        // n_Aexc_Dem is usually neglegible
        QString warning;
       if((((double)burst->n_Dem_Aex)/burst->numberOfPhotons)>0.1) {
           warning=warning.sprintf("Warning: burst %i has n_Aexc_Dem=%i"
                                   "and a total number of %i photons. Usually,"
                                   "n_Aexc_Dem is neglegible",
                                   idx, burst->n_Dem_Aex, burst->numberOfPhotons);
           AlexEvalLog::warning(warning);
       }
    }

    return EXIT_SUCCESS;

}

//
//
int statisticsFRET(FRETstatistics_t &FRETstatistics, const QVector<BurstDual> &burstVectorDual, const FRETparams_t &FRETparams )
{
    // calculate mean values
    if(burstVectorDual.isEmpty()) return EXIT_FAILURE;
    double PRmean=0;
    double PRvar=0;
    double Smean=0;
    double Svar=0;
    int nDonly=0;
    int nAonly=0;
    int nDual=0;
    unsigned long long quasiBulkN_Dem_Dex=0;
    unsigned long long quasiBulkN_Aem_Dex=0;
    unsigned long long quasiBulkN_Aem_Aex=0;
    FRETstatistics.meanBurstDuration=0;
    FRETstatistics.meanBurstSize=0;
    FRETstatistics.meanBurstSizeD=0;
    FRETstatistics.meanBurstSizeA=0;
    int count=0;
    for (register long long i=0; i<burstVectorDual.size(); ++i) {
        if (burstVectorDual.at(i).isMasked(FRETparams)) continue;
        switch(burstVectorDual.at(i).type) {
        case Dual:
            ++nDual;
            break;
        case Aonly:
            ++nAonly;
            FRETstatistics.meanBurstSizeA += burstVectorDual.at(i).burstSize();
            break;
        case Donly:
            ++nDonly;
            FRETstatistics.meanBurstSizeD += burstVectorDual.at(i).burstSize();
            break;
        default:
            break;
        }
        PRmean+= burstVectorDual.at(i).proximityRatio;
        PRvar += burstVectorDual.at(i).proximityRatio*burstVectorDual.at(i).proximityRatio;
        Smean += burstVectorDual.at(i).stoichiometryRatio;
        Svar  += burstVectorDual.at(i).stoichiometryRatio*burstVectorDual.at(i).stoichiometryRatio;
        FRETstatistics.meanBurstDuration += burstVectorDual.at(i).duration;
        FRETstatistics.meanBurstSize += burstVectorDual.at(i).burstSize();
        FRETstatistics.meanBurstRate += burstVectorDual.at(i).burstrate();
        quasiBulkN_Dem_Dex += burstVectorDual.at(i).n_Dem_Dex;
        quasiBulkN_Aem_Dex += burstVectorDual.at(i).n_Aem_Dex;
        quasiBulkN_Aem_Aex += burstVectorDual.at(i).n_Aem_Aex;
        ++count;
    }

    PRmean/= (double) count;
    PRvar  = PRvar/((double)count-1)-PRmean*PRmean;
    Smean /= (double) count;
    Svar   = Svar/((double)count-1)-Smean*Smean;
    FRETstatistics.Pmean=PRmean;
    FRETstatistics.Smean=Smean;
    FRETstatistics.Pstd  = sqrt(PRvar);
    FRETstatistics.Sstd   = sqrt(Svar);
    double durationTotal = FRETstatistics.meanBurstDuration;
    FRETstatistics.meanBurstDuration/=count;
    FRETstatistics.meanBurstSize/=count;
    FRETstatistics.meanBurstRate/=count;
    FRETstatistics.meanBurstSizeD/=nDonly;
    FRETstatistics.meanBurstSizeA/=nAonly;
    FRETstatistics.n   = count;
    FRETstatistics.nAonly   = nAonly;
    FRETstatistics.nDonly   = nDonly;
    FRETstatistics.nDual   = nDual;

    quasiBulkN_Aem_Aex *= FRETparams.scaleAcceptor;
    double f_Dem_Dex=(double)quasiBulkN_Dem_Dex - FRETparams.bg_Dem_Dex*durationTotal;
    double f_Aem_Dex=(double)quasiBulkN_Aem_Dex - FRETparams.bg_Aem_Dex*durationTotal;
    double f_Aem_Aex=(double)quasiBulkN_Aem_Aex - FRETparams.bg_Aem_Aex*durationTotal;
    double f_Dexc_gamma = FRETparams.gamma*f_Dem_Dex + (f_Aem_Dex - FRETparams.crosstalk*f_Dem_Dex - FRETparams.directExc*f_Aem_Aex);

    FRETstatistics.quasiBulkS = f_Dexc_gamma/(f_Dexc_gamma + f_Aem_Aex);
    FRETstatistics.quasiBulkP   = (f_Aem_Dex - FRETparams.crosstalk*f_Dem_Dex - FRETparams.directExc*f_Aem_Aex)/(f_Dexc_gamma);

//    qDebug()<<FRETparams.toString();
//    qDebug("PR=%.3f±%.3f",PRmean, PRstd);
//    qDebug("S=%.3f±%.3f", Smean , Sstd );
    return count;
}


int analysisFCStotal(const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor, const QVector<BurstDual> &burstVectorDual, const FRETparams_t &FRETparams, const FCSparams_t &FCSparams )
{
    int e=EXIT_FAILURE;

    if(FCSparams.FCSMethod==FCSparams_t::FCScomplete) {
        e=FCSmultiTauBinned(arrivalTimeDonor, arrivalTimeAcceptor, FCSparams);
    } else if(FCSparams.FCSMethod==FCSparams_t::FCSstep) {
        QVector<BurstDual> selected;
        if(selectSubpopulation(selected,burstVectorDual,FRETparams)==EXIT_FAILURE) return EXIT_FAILURE;
//        e=FCSmultiTauStepwise(arrivalTimeDonor, arrivalTimeAcceptor, selected, FCSparams);;
//        e=FCSmultiTauStepwise()
        qFatal("error");
    } else {
        AlexEvalLog::warning("unknown FCSMethod");
        return EXIT_FAILURE;
    }

    return e;
}



int FCSmultiTauBinned(const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor, const FCSparams_t &FCSparams)
{
    qDebug("species-selective FCS");
    if(arrivalTimeDonor.isEmpty()||arrivalTimeAcceptor.isEmpty()) {AlexEvalLog::warning("vectors empty");return EXIT_FAILURE;}

    // parameter
    int S=FCSparams.S; // blocks
    int m=FCSparams.m; // multiply delta tau
    int P=FCSparams.P; // tau values per block
    double binwidth=FCSparams.MinLagTime;

    uint32_t ntau=S*P;
    long*tau = new long[ntau+5];
    statisticsAutocorrelateCreateMultiTau(tau,S,m,P); // create log-spaced tau values in array tau. note that the values refer to the index in the array to correlate, i.e. in this case to the number of bins.


    // bin arrival times
    int nbins=(int)qMax(((arrivalTimeDonor.last()-arrivalTimeDonor.first())/binwidth),((arrivalTimeAcceptor.last()-arrivalTimeAcceptor.first())/binwidth));
    qDebug("number of bins is %i",nbins);

    gsl_histogram * histDonor = gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform (histDonor, arrivalTimeDonor.first(), arrivalTimeDonor.last());
    for (int ii=0;ii<arrivalTimeDonor.size();ii++) gsl_histogram_increment (histDonor, arrivalTimeDonor[ii]);

    gsl_histogram * histAcceptor = gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform (histAcceptor, arrivalTimeAcceptor.first(), arrivalTimeAcceptor.last());
    for (int ii=0;ii<arrivalTimeAcceptor.size();ii++) gsl_histogram_increment (histAcceptor, arrivalTimeAcceptor[ii]);


    // correlate the bins: autocorreltion for each channel and cross correlation between the two
    double* autoCorrDonor=new double[ntau];
    double* autoCorrAcceptor=new double[ntau];
    double* crossCorr=new double[ntau];
    uint32_t n=histDonor->n;
//    if(n!=nbins) qWarning("n!=nbins");
    // double* data=histDonor->bin; // in the gsl, bin is a pointer to a double


    qDebug("autoCorrDonor");
    statisticsCrosscorrelateMultiTauSymmetric<double, double>(autoCorrDonor,histDonor->bin,histDonor->bin,n,tau,ntau);
    qDebug("autoCorrAcceptor");
    statisticsCrosscorrelateMultiTauSymmetric<double, double>(autoCorrAcceptor,histAcceptor->bin,histAcceptor->bin,n,tau,ntau);
    qDebug("crossCorr");
    statisticsCrosscorrelateMultiTauSymmetric<double, double>(crossCorr,histDonor->bin,histAcceptor->bin,n,tau,ntau);

    qDebug("tau in ms\tDonor ACF\tAcceptor ACF\tCCF");
    for (unsigned int i=0; i<ntau; i++) {
        qDebug("%f\t%f\t%f\t%f", tau[i]*binwidth*1e3, autoCorrDonor[i],autoCorrAcceptor[i],crossCorr[i]);
    }

    gsl_histogram_free(histDonor);
    gsl_histogram_free(histAcceptor);

    return EXIT_SUCCESS;
}




// 12.6.13 "fill gap bins" FIXME unfinished
int FCSstepFill(QVector<double> &tau, QVector<double> &corrDonor, QVector<double> &corrAcceptor, QVector<double> &crossCorr, QVector<double> &corrSum, const QVector<double> &arrivalTimeDonor,const QVector<double> &arrivalTimeAcceptor, const QVector<double> markerTimeDexc,  const QVector<double> markerTimeAexc, const QVector<BurstDual> &burstVectorDual, const FCSparams_t &FCSparams )
{
    qDebug("species-selective FCS stepwise with %i bursts",burstVectorDual.size());
    if(arrivalTimeDonor.isEmpty()||arrivalTimeAcceptor.isEmpty()||burstVectorDual.isEmpty()) {AlexEvalLog::warning("vectors empty");return EXIT_FAILURE;}
    if((tau.size()>0)||(corrDonor.size()>0)||(crossCorr.size()>0)||(corrAcceptor.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        tau.clear();
        corrDonor.clear();
        corrAcceptor.clear();
        crossCorr.clear();
    }
    int startD,endD,startA,endA;

    // parameter
    int S=FCSparams.S; // blocks
    int m=FCSparams.m; // multiply delta tau
    int P=FCSparams.P; // tau values per block
    double binwidth=FCSparams.MinLagTime ;
    int offset=FCSparams.offset;
    double rate=1000; // approximate background rate
    double lagTime=FCSparams.CorrelationWindow+2*offset/rate;

    int nbins=(int)(lagTime/binwidth+0.5);
    qDebug("nbins=%i", nbins);
    qDebug("LagTime=%f",lagTime);
    gsl_histogram * histDonor = gsl_histogram_alloc(nbins);
    gsl_histogram * histAcceptor = gsl_histogram_alloc(nbins);

    MultiTauCorrelator<double, double> corrD(S,m,P,1);
    MultiTauCorrelator<double, double> corrA(S,m,P,1);
    MultiTauCorrelator<double, double> ccorr(S,m,P,1);
    MultiTauCorrelator<double, double> corrS(S,m,P,1);


    gsl_histogram_set_ranges_uniform (histDonor, 0, lagTime);
    gsl_histogram_set_ranges_uniform (histAcceptor, 0, lagTime);

    int correlatedPhotonsD=0;
    int correlatedPhotonsA=0;
    double meanIntensityD=0;
    double meanIntensityA=0;
    int markeridx=0;
    int lastMarker=0;

    markeridx=0;

    for(int i=0; i<burstVectorDual.size(); ++i) { // concatenate all bursts

        lastMarker=markeridx;
        startD=qMax(burstVectorDual.at(i).startIdxDonor-offset,0);
        endD=qMin(startD+burstVectorDual.at(i).n_Dem()+offset, arrivalTimeDonor.size());
        startA=qMax(burstVectorDual.at(i).startIdxAcceptor-offset,0);
        endA=qMin(startA+burstVectorDual.at(i).n_Aem()+offset,arrivalTimeAcceptor.size());

        double startTime=qMin(arrivalTimeDonor[startD],arrivalTimeAcceptor[startA]);

        int j = startD;
        while(markerTimeDexc[markeridx+1]<burstVectorDual.at(i).startTime) markeridx++;
        while(markerTimeDexc[markeridx]<=burstVectorDual.at(i).endTime()) {
            while ( (markerTimeDexc[markeridx]>arrivalTimeDonor[j]) && (j<endD)) ++j;
            while ((markerTimeDexc[markeridx]<arrivalTimeDonor[j]) && (arrivalTimeDonor[j]<=markerTimeAexc[markeridx]) && (j<endD)) { //only during donor excitation
                gsl_histogram_increment(histDonor, arrivalTimeDonor[j]-startTime); // convert photon times to intensity
                ++j;
                correlatedPhotonsD++;
            }
            markeridx++;
        }

        markeridx=lastMarker;

        j = startA;
        while(markerTimeDexc[markeridx+1]<burstVectorDual.at(i).startTime) markeridx++;
        while(markerTimeDexc[markeridx]<=burstVectorDual.at(i).endTime()) {
            while ( (markerTimeDexc[markeridx]>arrivalTimeAcceptor[j]) && (j<endA)) ++j;
            while ((markerTimeDexc[markeridx]<arrivalTimeAcceptor[j]) && (arrivalTimeAcceptor[j]<=markerTimeAexc[markeridx]) && (j<endA)) { //only during donor excitation
                gsl_histogram_increment(histAcceptor, arrivalTimeAcceptor[j]-startTime); // convert photon times to intensity
                ++j;
                correlatedPhotonsA++;
            }
            markeridx++;
        }


        // "fill the gaps"
        // histogram (=intensity timetrace) of the alternating laser excitation scheme
        double T1,T2,T3,T4;
        T1=100e-6;
        T2=20e-6;
        T3=100e-6;
        T4=20e-6;

        double Tend_donor=T1;
        double Tend_pause1=T1+T2;
        double Tend_acceptor=T1+T2+T3;
        double Tend_pause2=T1+T2+T3+T4;
        size_t ii=0;
        int n=0;

        while(ii<histDonor->n) {
            while((histDonor->range[ii]<=(Tend_donor+n*Tend_pause2)) && ii<histDonor->n) { //while(histOsc->range[i]<T1) histOsc->bin[i++]++;
                histDonor->bin[ii]=1;
                histAcceptor->bin[ii]=0;
                ii++;
            }
            while((histDonor->range[ii]<=(Tend_pause1+n*Tend_pause2)) && ii<histDonor->n) {
                histDonor->bin[ii]=0;
                histAcceptor->bin[ii]=0;
                ii++;
            }
            while((histDonor->range[ii]<=(Tend_acceptor+n*Tend_pause2)) && ii<histDonor->n) {
                histDonor->bin[ii]=0;
                histAcceptor->bin[ii]=1;
                ii++;
            }
            while((histDonor->range[ii]<=(Tend_pause2+n*Tend_pause2)) && ii<histDonor->n) {
                histDonor->bin[ii]=0;
                histAcceptor->bin[ii]=0;
                ii++;
            }
            n++;
        }


//        if(i==2) qDebug("tau in us\tDonor hist count");
        for (size_t k=0; k<histDonor->n; ++k) {
//            if(i==2) qDebug()<<histDonor->range[k]*1e6<<"\t"<<histDonor->bin[k];
            corrD.correlate_step(histDonor->bin[k]);
            corrA.correlate_step(histAcceptor->bin[k]);
            ccorr.crosscorrelate_step(histDonor->bin[k], histAcceptor->bin[k]);
            corrS.correlate_step(histDonor->bin[k] + histAcceptor->bin[k]);
        }

        meanIntensityD+=gsl_histogram_mean(histDonor);
        meanIntensityA+=gsl_histogram_mean(histAcceptor);

        gsl_histogram_reset(histAcceptor);
        gsl_histogram_reset(histDonor);
    }
    qDebug()<<"correlated"<<correlatedPhotonsD<<"donor photons and "<<correlatedPhotonsA<<"acceptor photons";
    meanIntensityD/=burstVectorDual.size();
    meanIntensityA/=burstVectorDual.size();

    corrD.normalize();
    corrA.normalize();
    ccorr.crossnormalize();
    corrS.normalize();


    //correction factor for background
    QVector<double> corrDonorCorrected;

    rate=FCSparams.backgroundAcceptor;
    qDebug()<<"binwidth*rateA="<<binwidth*rate;
    double backgroundCorrectionA=pow(binwidth*rate,2)/pow(meanIntensityA,2) - 2*binwidth*rate/meanIntensityA;
    rate=FCSparams.backgroundDonor;
    qDebug()<<"binwidth*rateD="<<binwidth*rate;
    double backgroundCorrectionD=pow(binwidth*rate,2)/pow(meanIntensityD,2) - 2*binwidth*rate/meanIntensityD;
    qDebug()<<"meanIntensityD="<<meanIntensityD;
    qDebug()<<"meanIntensityA="<<meanIntensityA;
    qDebug()<<"backgroundCorrectionFactorD="<<backgroundCorrectionD;
    qDebug()<<"backgroundCorrectionFactorA="<<backgroundCorrectionA;


    // save correlation functions in vectors
    unsigned int cN=corrD.getSlots();
    for (unsigned int i=0;i<cN;++i) tau.push_back(corrD.getCorTau()[i]*binwidth);
    for (unsigned int i=0;i<cN;++i) corrDonor.push_back(corrD.getCor()[i]);
    for (unsigned int i=0;i<cN;++i) corrAcceptor.push_back(corrA.getCor()[i]);
    for (unsigned int i=0;i<cN;++i) crossCorr.push_back(ccorr.getCor()[i]);
    for (unsigned int i=0;i<cN;++i) corrSum.push_back(corrS.getCor()[i]);


#ifdef MYDEBUG
    if ((cN==corrA.getSlots())&&(cN==ccorr.getSlots())) {
        qDebug("tau in ms\tDonor_ACF\tAcceptor_ACF\tCCF\tSum\tcorrD_corrected");
        for (unsigned int i=0; i<cN; ++i) {
            qDebug("%lf\t%lf\t%lf\t%lf\t%lf\t%lf", tau[i]*1e3, corrD.getCor()[i],corrA.getCor()[i],crossCorr[i],corrSum[i],corrDonorCorrected.at(i));
        }
    } else AlexEvalLog::warning("error");
#endif

    // free memory
    gsl_histogram_free(histDonor);
    gsl_histogram_free(histAcceptor);

    return EXIT_SUCCESS;
}



// 07.02.2014
int FCSmultiTauStepwiseSimple(QVector<double> &tau, QVector<double> &corr, const QVector<double> &arrivalTimes, const FCSparams_t &FCSparams )
{
    qDebug("species-selective FCS stepwise with %i photons",arrivalTimes.size());
    if(arrivalTimes.isEmpty()) {AlexEvalLog::warning("arrivalTimes vector empty");return EXIT_FAILURE;}
    if((tau.size()>0)||(corr.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        tau.clear();
        corr.clear();
    }

    // parameter
    int S=FCSparams.S; // blocks
    int m=FCSparams.m; // multiply delta tau
    int P=FCSparams.P; // tau values per block
    double binwidth=FCSparams.MinLagTime ;

    MultiTauCorrelator<double, double> corrD(S,m,P,1);

    int correlatedPhotons=0;
    double t=0;
    double tEnd=arrivalTimes.last();
    int n=arrivalTimes.size();
    int counter=0;

    int photonIdx=0;
    while(t<tEnd) {
        counter=0;
        while(photonIdx<n && arrivalTimes.at(photonIdx)<t) {
            ++counter;
            ++photonIdx;
        }
        t+=binwidth;
        correlatedPhotons+=counter;
        corrD.correlate_step(counter);
    }

    qDebug()<<"correlated"<<correlatedPhotons<<"photons.";

    corrD.normalize();


    // save correlation functions in vectors
    unsigned int cN=corrD.getSlots();
    for (unsigned int i=0;i<cN;++i) tau.push_back(corrD.getCorTau()[i]*binwidth);
    for (unsigned int i=0;i<cN;++i) corr.push_back(corrD.getCor()[i]); //(corrD.getCor()[i]*meanIntensityD*meanIntensityD+binwidth*rate*(binwidth*rate-2*meanIntensityD))/(corrOscD.getCor()[i]*(meanIntensityD*meanIntensityD+binwidth*rate*(binwidth*rate-2*meanIntensityD));

//#define MYDEBUG
#ifdef MYDEBUG
//    if ((cN==corrA.getSlots())&&(cN==ccorr.getSlots())) {
//        qDebug("tau in ms\tACF_D\tACF_A\tCCF_DA\tSum_DA\tACF_D_boxed\tACF_A_boxed\tCCF_DA_boxed\tSum_DA_boxed");
//        for (unsigned int i=0; i<cN; ++i) {
//            qDebug("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf", tau[i]*1e3, corr1.getCor()[i],corrA.getCor()[i],ccorr.getCor()[i],corrS.getCor()[i],corrDonor.at(i),corrAcceptor.at(i),crossCorr.at(i),corrSum.at(i));
//        }
//    } else
        AlexEvalLog::warning("error");
#endif

    return EXIT_SUCCESS;
}



// 12.6.13 "boxed mean filter"
int FCSmultiTauStepwise(QVector<double> &tau, QVector<double> &corrDonor, QVector<double> &corrAcceptor, QVector<double> &crossCorr, QVector<double> &corrSum, const QVector<double> &arrivalTimeDonor,const QVector<double> &arrivalTimeAcceptor, const QVector<double> markerTimeDexc,  const QVector<double> markerTimeAexc, const QVector<BurstDual> &burstVectorDual, const FCSparams_t &FCSparams, bool doBoxFiltering )
{
    qDebug("species-selective FCS stepwise with %i bursts",burstVectorDual.size());
    if(arrivalTimeDonor.isEmpty()||arrivalTimeAcceptor.isEmpty()||burstVectorDual.isEmpty()) {AlexEvalLog::warning("vectors empty");return EXIT_FAILURE;}
    if((tau.size()>0)||(corrDonor.size()>0)||(crossCorr.size()>0)||(corrAcceptor.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        tau.clear();
        corrDonor.clear();
        corrAcceptor.clear();
        crossCorr.clear();
    }
    int startD,endD,startA,endA;

    // parameter
    int S=FCSparams.S; // blocks
    int m=FCSparams.m; // multiply delta tau
    int P=FCSparams.P; // tau values per block
    double binwidth=FCSparams.MinLagTime ;
    int offset=FCSparams.offset;
    double rate=1000; // approximate background rate
    double lagTime=FCSparams.CorrelationWindow+offset/rate;

    int nbins=(int)(lagTime/binwidth+0.5);
    qDebug("nbins=%i", nbins);
    qDebug("LagTime=%f",lagTime);
    gsl_histogram * histDonor = gsl_histogram_alloc(nbins);
    gsl_histogram * histAcceptor = gsl_histogram_alloc(nbins);

    MultiTauCorrelator<double, double> corrD(S,m,P,1);
    MultiTauCorrelator<double, double> corrA(S,m,P,1);
    MultiTauCorrelator<double, double> ccorr(S,m,P,1);
    MultiTauCorrelator<double, double> corrS(S,m,P,1);


    gsl_histogram_set_ranges_uniform (histDonor, 0, lagTime);
    gsl_histogram_set_ranges_uniform (histAcceptor, 0, lagTime);

    int correlatedPhotonsD=0;
    int correlatedPhotonsA=0;
    double meanIntensityD=0;
    double meanIntensityA=0;
    int markeridx=0;
    int lastMarker=0;

    while(markerTimeDexc[markeridx+1]<burstVectorDual.at(0).startTime) markeridx++;
    double startTime=markerTimeDexc[markeridx]; // first start time


    for(int i=0; i<burstVectorDual.size(); ++i) { // concatenate all bursts

        lastMarker=markeridx;
        startD=qMax(burstVectorDual.at(i).startIdxDonor-offset,0);
        endD=qMin(startD+burstVectorDual.at(i).n_Dem()+offset, arrivalTimeDonor.size());
        startA=qMax(burstVectorDual.at(i).startIdxAcceptor-offset,0);
        endA=qMin(startA+burstVectorDual.at(i).n_Aem()+offset,arrivalTimeAcceptor.size());

//        double startTime=qMin(arrivalTimeDonor[startD],arrivalTimeAcceptor[startA]);

        int j = startD;

        while(markerTimeDexc[markeridx]<=burstVectorDual.at(i).endTime()) {
            while ( (markerTimeDexc[markeridx]>arrivalTimeDonor[j]) && (j<endD)) ++j; // skip photons in acceptor excitation periods and gaps
            while ((markerTimeDexc[markeridx]<arrivalTimeDonor[j]) && (arrivalTimeDonor[j]<=markerTimeAexc[markeridx]) && (j<endD)) { //only during donor excitation
                gsl_histogram_increment(histDonor, arrivalTimeDonor[j]-startTime); // convert photon times to intensity
                ++j;
                correlatedPhotonsD++;
            }
            markeridx++;
        }

        markeridx=lastMarker;

        j = startA;
        while(markerTimeDexc[markeridx+1]<burstVectorDual.at(i).startTime) markeridx++;
        while(markerTimeDexc[markeridx]<=burstVectorDual.at(i).endTime()) {
            while ( (markerTimeDexc[markeridx]>arrivalTimeAcceptor[j]) && (j<endA)) ++j;
            while ((markerTimeDexc[markeridx]<arrivalTimeAcceptor[j]) && (arrivalTimeAcceptor[j]<=markerTimeAexc[markeridx]) && (j<endA)) { //only during donor excitation
                gsl_histogram_increment(histAcceptor, arrivalTimeAcceptor[j]-startTime); // convert photon times to intensity
                ++j;
                correlatedPhotonsA++;
            }
            markeridx++;
        }


//        if(i+1<burstVectorDual.size()) if((burstVectorDual.at(i+1).startTime-startTime)<=lagTime) {
//            qDebug()<<"would have to include next burst. startTime"<<(burstVectorDual.at(i+1).startTime-startTime)*1e3<<"ms";
//        }
//        if(i+1<burstVectorDual.size()) if((burstVectorDual.at(i+1).endTime()-startTime)<=lagTime) {
//            i++;// include also the next burst
////            qDebug()<<"include next burst. endTime"<<(burstVectorDual.at(i).endTime()-startTime)*1e3<<"ms";
//            markeridx=lastMarker; // reset marker
//            continue;
//        }



//        if(i==2) qDebug("tau in us\tDonor hist count");
        for (size_t k=0; k<histDonor->n; ++k) {
//            if(i==2) qDebug()<<histDonor->range[k]*1e6<<"\t"<<histDonor->bin[k];
            corrD.correlate_step(histDonor->bin[k]);
            corrA.correlate_step(histAcceptor->bin[k]);
            ccorr.crosscorrelate_step(histDonor->bin[k], histAcceptor->bin[k]);
            corrS.correlate_step(histDonor->bin[k] + histAcceptor->bin[k]);
        }

        meanIntensityD+=gsl_histogram_mean(histDonor);
        meanIntensityA+=gsl_histogram_mean(histAcceptor);

        gsl_histogram_reset(histAcceptor);
        gsl_histogram_reset(histDonor);

        if(i+1<burstVectorDual.size()) while(markerTimeDexc[markeridx+1]<burstVectorDual.at(i+1).startTime) markeridx++;
        startTime=markerTimeDexc[markeridx];
    }


    qDebug()<<"correlated"<<correlatedPhotonsD<<"donor photons and "<<correlatedPhotonsA<<"acceptor photons";
    meanIntensityD/=burstVectorDual.size();
    meanIntensityA/=burstVectorDual.size();

    corrD.normalize();
    corrA.normalize();
    ccorr.crossnormalize();
    corrS.normalize();




    // save correlation functions in vectors
    unsigned int cN=corrD.getSlots();
    for (unsigned int i=0;i<cN;++i) tau.push_back(corrD.getCorTau()[i]*binwidth);
    for (unsigned int i=0;i<cN;++i) corrDonor.push_back(corrD.getCor()[i]); //(corrD.getCor()[i]*meanIntensityD*meanIntensityD+binwidth*rate*(binwidth*rate-2*meanIntensityD))/(corrOscD.getCor()[i]*(meanIntensityD*meanIntensityD+binwidth*rate*(binwidth*rate-2*meanIntensityD));
    for (unsigned int i=0;i<cN;++i) corrAcceptor.push_back(corrA.getCor()[i]);
    for (unsigned int i=0;i<cN;++i) crossCorr.push_back(ccorr.getCor()[i]);
    for (unsigned int i=0;i<cN;++i) corrSum.push_back(corrS.getCor()[i]);


#ifdef MYDEBUG
    if ((cN==corrA.getSlots())&&(cN==ccorr.getSlots())) {
        qDebug("tau in ms\tACF_D\tACF_A\tCCF_DA\tSum_DA\tACF_D_boxed\tACF_A_boxed\tCCF_DA_boxed\tSum_DA_boxed");
        for (unsigned int i=0; i<cN; ++i) {
            qDebug("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf", tau[i]*1e3, corrD.getCor()[i],corrA.getCor()[i],ccorr.getCor()[i],corrS.getCor()[i],corrDonor.at(i),corrAcceptor.at(i),crossCorr.at(i),corrSum.at(i));
        }
    } else AlexEvalLog::warning("error");
#endif

    // free memory
    gsl_histogram_free(histDonor);
    gsl_histogram_free(histAcceptor);

    return EXIT_SUCCESS;
}



// 10.6.13 "divide by oscillation ACF"
int FCSstepOsc(QVector<double> &tau, QVector<double> &corrDonor, QVector<double> &corrAcceptor, QVector<double> &crossCorr, QVector<double> &corrSum, const QVector<double> &arrivalTimeDonor,const QVector<double> &arrivalTimeAcceptor, const QVector<double> markerTimeDexc,  const QVector<double> markerTimeAexc, const QVector<BurstDual> &burstVectorDual, const FCSparams_t &FCSparams )
{
    qDebug("species-selective FCS stepwise with %i bursts",burstVectorDual.size());
    if(arrivalTimeDonor.isEmpty()||arrivalTimeAcceptor.isEmpty()||burstVectorDual.isEmpty()) {AlexEvalLog::warning("vectors empty");return EXIT_FAILURE;}
    if((tau.size()>0)||(corrDonor.size()>0)||(crossCorr.size()>0)||(corrAcceptor.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        tau.clear();
        corrDonor.clear();
        corrAcceptor.clear();
        crossCorr.clear();
    }
    int startD,endD,startA,endA;

    // parameter
    int S=FCSparams.S; // blocks
    int m=FCSparams.m; // multiply delta tau
    int P=FCSparams.P; // tau values per block
    double binwidth=FCSparams.MinLagTime ;
    int offset=FCSparams.offset;
    double rate=1000; // approximate background rate
    double lagTime=FCSparams.CorrelationWindow+offset/rate;

    int nbins=(int)(lagTime/binwidth+0.5);
    qDebug("nbins=%i", nbins);
    qDebug("LagTime=%f",lagTime);
    gsl_histogram * histDonor = gsl_histogram_alloc(nbins);
    gsl_histogram * histAcceptor = gsl_histogram_alloc(nbins);

    MultiTauCorrelator<double, double> corrD(S,m,P,1);
    MultiTauCorrelator<double, double> corrA(S,m,P,1);
    MultiTauCorrelator<double, double> ccorr(S,m,P,1);
    MultiTauCorrelator<double, double> corrS(S,m,P,1);



    gsl_histogram_set_ranges_uniform (histDonor, 0, lagTime);
    gsl_histogram_set_ranges_uniform (histAcceptor, 0, lagTime);

    int correlatedPhotonsD=0;
    int correlatedPhotonsA=0;
    double meanIntensityD=0;
    double meanIntensityA=0;
    int markeridx=0;
    int lastMarker=0;



    /*
    QBitArray DemIsDexc(arrivalTimeDonor.size());
    QBitArray AemIsDexc(arrivalTimeAcceptor.size());
    int lastD=0,lastA=0;
    while(markeridx<markerTimeAexc.size()) {
        while((markerTimeDexc[markeridx]<arrivalTimeDonor[lastD]) && (arrivalTimeDonor[lastD]<=markerTimeAexc[markeridx])) {
            DemIsDexc.setBit(lastD);
            lastD++;
        }

    }
    */


//    double periodDonorMean=(markerTimeDexc.last()- markerTimeDexc.first())/(markerTimeDexc.size()-1);
//    double periodAcceptorMean=(markerTimeAexc.last()- markerTimeAexc.first())/(markerTimeAexc.size()-1);



    markeridx=0;

    for(int i=0; i<burstVectorDual.size(); ++i) { // concatenate all bursts

        lastMarker=markeridx;
        startD=qMax(burstVectorDual.at(i).startIdxDonor-offset,0);
        endD=qMin(startD+burstVectorDual.at(i).n_Dem()+offset, arrivalTimeDonor.size());
        startA=qMax(burstVectorDual.at(i).startIdxAcceptor-offset,0);
        endA=qMin(startA+burstVectorDual.at(i).n_Aem()+offset,arrivalTimeAcceptor.size());

        double startTime=qMin(arrivalTimeDonor[startD],arrivalTimeAcceptor[startA]);

        int j = startD;
        while(markerTimeDexc[markeridx+1]<burstVectorDual.at(i).startTime) markeridx++;
        while(markerTimeDexc[markeridx]<=burstVectorDual.at(i).endTime()) {
            while ( (markerTimeDexc[markeridx]>arrivalTimeDonor[j]) && (j<endD)) ++j;
            while ((markerTimeDexc[markeridx]<arrivalTimeDonor[j]) && (arrivalTimeDonor[j]<=markerTimeAexc[markeridx]) && (j<endD)) { //only during donor excitation
                gsl_histogram_increment(histDonor, arrivalTimeDonor[j]-startTime); // convert photon times to intensity
                ++j;
                correlatedPhotonsD++;
            }
            markeridx++;
        }

        markeridx=lastMarker;

        j = startA;
        while(markerTimeDexc[markeridx+1]<burstVectorDual.at(i).startTime) markeridx++;
        while(markerTimeDexc[markeridx]<=burstVectorDual.at(i).endTime()) {
            while ( (markerTimeDexc[markeridx]>arrivalTimeAcceptor[j]) && (j<endA)) ++j;
            while ((markerTimeDexc[markeridx]<arrivalTimeAcceptor[j]) && (arrivalTimeAcceptor[j]<=markerTimeAexc[markeridx]) && (j<endA)) { //only during donor excitation
                gsl_histogram_increment(histAcceptor, arrivalTimeAcceptor[j]-startTime); // convert photon times to intensity
                ++j;
                correlatedPhotonsA++;
            }
            markeridx++;
        }

//        if(i==2) qDebug("tau in us\tDonor hist count");
        for (size_t k=0; k<histDonor->n; ++k) {
//            if(i==2) qDebug()<<histDonor->range[k]*1e6<<"\t"<<histDonor->bin[k];
            corrD.correlate_step(histDonor->bin[k]);
            corrA.correlate_step(histAcceptor->bin[k]);
            ccorr.crosscorrelate_step(histDonor->bin[k], histAcceptor->bin[k]);
            corrS.correlate_step(histDonor->bin[k] + histAcceptor->bin[k]);
        }

        meanIntensityD+=gsl_histogram_mean(histDonor);
        meanIntensityA+=gsl_histogram_mean(histAcceptor);

        gsl_histogram_reset(histAcceptor);
        gsl_histogram_reset(histDonor);
    }
    qDebug()<<"correlated"<<correlatedPhotonsD<<"donor photons and "<<correlatedPhotonsA<<"acceptor photons";
    meanIntensityD/=burstVectorDual.size();
    meanIntensityA/=burstVectorDual.size();

    corrD.normalize();
    corrA.normalize();
    ccorr.crossnormalize();
    corrS.normalize();



    MultiTauCorrelator<double, double> corrOscD(S,m,P,1);
    MultiTauCorrelator<double, double> corrOscA(S,m,P,1);
    // histogram (=intensity timetrace) of the alternating laser excitation scheme


    double Tend_donor=FCSparams.tDex;
    double Tend_pause1=FCSparams.tDex+FCSparams.tGapDexAex;
    double Tend_acceptor=FCSparams.tDex+FCSparams.tGapDexAex+FCSparams.tAex;
    double Tend_pause2=FCSparams.tDex+FCSparams.tGapDexAex+FCSparams.tAex+FCSparams.tGapAexDex;

    gsl_histogram * histOscDonor = gsl_histogram_alloc(nbins); // oscillation through laser alternation
    gsl_histogram * histOscAcceptor = gsl_histogram_alloc(nbins); // oscillation through laser alternation
    gsl_histogram_set_ranges_uniform (histOscDonor, 0, lagTime);
    gsl_histogram_set_ranges_uniform (histOscAcceptor, 0, lagTime);
//    for(int i=0;i<nbins;++i) {
//        gsl_histogram_increment(histOsc,);
//    }

    size_t i=0;
    double n=0;
    while(i<histOscDonor->n) {
        while((histOscDonor->range[i]<=(Tend_donor+n*Tend_pause2)) && i<histOscDonor->n) { //while(histOsc->range[i]<T1) histOsc->bin[i++]++;
            histOscDonor->bin[i]=1;
            histOscAcceptor->bin[i]=0;
            i++;
        }
        while((histOscDonor->range[i]<=(Tend_pause1+n*Tend_pause2)) && i<histOscDonor->n) {
            histOscDonor->bin[i]=0;
            histOscAcceptor->bin[i]=0;
            i++;
        }
        while((histOscDonor->range[i]<=(Tend_acceptor+n*Tend_pause2)) && i<histOscDonor->n) {
            histOscDonor->bin[i]=0;
            histOscAcceptor->bin[i]=1;
            i++;
        }
        while((histOscDonor->range[i]<=(Tend_pause2+n*Tend_pause2)) && i<histOscDonor->n) {
            histOscDonor->bin[i]=0;
            histOscAcceptor->bin[i]=0;
            i++;
        }
        n++;
    }
    double varOsc=0; // FIXME
    for (size_t k=0; k<histOscDonor->n; ++k) {
        corrOscD.correlate_step(histOscDonor->bin[k]);
        varOsc+=histOscDonor->bin[k]*histOscDonor->bin[k];
//        qDebug()<<histOscDonor->range[k]*1e6<<"\t"<<histOscDonor->bin[k];
    }
    corrOscD.normalize();
    varOsc=varOsc/histOscDonor->n;
    qDebug()<<"varOsc="<<varOsc;

    corrOscA.correlate_series(histOscAcceptor->bin,histOscAcceptor->n);
    corrOscA.normalize();

    //correction factor for background
    QVector<double> corrDonorCorrected;

    rate=FCSparams.backgroundAcceptor;
    qDebug()<<"binwidth*rateA="<<binwidth*rate;
    double backgroundCorrectionA=pow(binwidth*rate,2)/pow(meanIntensityA,2) - 2*binwidth*rate/meanIntensityA;
    rate=FCSparams.backgroundDonor;
    qDebug()<<"binwidth*rateD="<<binwidth*rate;
    double backgroundCorrectionD=pow(binwidth*rate,2)/pow(meanIntensityD,2) - 2*binwidth*rate/meanIntensityD;
    qDebug()<<"meanIntensityD="<<meanIntensityD;
    qDebug()<<"meanIntensityA="<<meanIntensityA;
    qDebug()<<"backgroundCorrectionFactorD="<<backgroundCorrectionD;
    qDebug()<<"backgroundCorrectionFactorA="<<backgroundCorrectionA;


    // save correlation functions in vectors
    QVector<double> corrOscVec;
    for (unsigned int i=0;i<corrOscD.getSlots();++i) corrOscVec.push_back(corrOscD.getCor()[i]);
    unsigned int cN=corrD.getSlots();
    for (unsigned int i=0;i<cN;++i) tau.push_back(corrD.getCorTau()[i]*binwidth);
    for (unsigned int i=0;i<cN;++i) corrDonor.push_back((corrD.getCor()[i]+backgroundCorrectionD)/(corrOscD.getCor()[i]*(1 + backgroundCorrectionD))); //(corrD.getCor()[i]*meanIntensityD*meanIntensityD+binwidth*rate*(binwidth*rate-2*meanIntensityD))/(corrOscD.getCor()[i]*(meanIntensityD*meanIntensityD+binwidth*rate*(binwidth*rate-2*meanIntensityD));
    for (unsigned int i=0;i<cN;++i) corrAcceptor.push_back((corrA.getCor()[i]+backgroundCorrectionA)/(corrOscA.getCor()[i]*(1 + backgroundCorrectionA)));
    for (unsigned int i=0;i<cN;++i) crossCorr.push_back(ccorr.getCor()[i]);
    for (unsigned int i=0;i<cN;++i) corrSum.push_back(corrS.getCor()[i]);



#ifdef MYDEBUG
    if ((cN==corrA.getSlots())&&(cN==ccorr.getSlots())) {
        qDebug("tau in ms\tDonor_ACF\tAcceptor_ACF\tCCF\tSum\tACF_Osc_D\tACF_Osc_A");
        for (unsigned int i=0; i<cN; ++i) {
            qDebug("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf", tau[i]*1e3, corrD.getCor()[i],corrA.getCor()[i],crossCorr[i],corrSum[i],corrOscD.getCor()[i],corrOscA.getCor()[i]);
        }
    } else AlexEvalLog::warning("error");
#endif

    // free memory
    gsl_histogram_free(histDonor);
    gsl_histogram_free(histAcceptor);

    return EXIT_SUCCESS;
}




/// 06.06.2013
int FCSstepOld(QVector<double> &tau, QVector<double> &corrDonor, QVector<double> &corrAcceptor, QVector<double> &crossCorr,const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor, const QVector<BurstDual> &burstVectorDual, const FCSparams_t &FCSparams )
{
    qDebug("Old species-selective FCS stepwise");
    if(arrivalTimeDonor.isEmpty()||arrivalTimeAcceptor.isEmpty()||burstVectorDual.isEmpty()) {AlexEvalLog::warning("vectors empty");return EXIT_FAILURE;}
    if((tau.size()>0)||(corrDonor.size()>0)||(crossCorr.size()>0)||(corrAcceptor.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        tau.clear();
        corrDonor.clear();
        corrAcceptor.clear();
        crossCorr.clear();
    }
    int start,end;


    // parameter
    int S=FCSparams.S; // blocks
    int m=FCSparams.m; // multiply delta tau
    int P=FCSparams.P; // tau values per block
    double binwidth=FCSparams.MinLagTime ;
    int offset=FCSparams.offset;
    double rate=1000; // approximate background rate
    double lagTime=FCSparams.CorrelationWindow;

    int nbins=(int)(lagTime/binwidth+0.5);
    qDebug("nbins=%i", nbins);
    qDebug("LagTime=%f",lagTime);
    gsl_histogram * histDonor = gsl_histogram_alloc(nbins);
    gsl_histogram * histAcceptor = gsl_histogram_alloc(nbins);

    MultiTauCorrelator<double, double> corrD(S,m,P,1);
    MultiTauCorrelator<double, double> corrA(S,m,P,1);
    MultiTauCorrelator<double, double> ccorr(S,m,P,1);


    gsl_histogram_set_ranges_uniform (histDonor, 0, lagTime);
    gsl_histogram_set_ranges_uniform (histAcceptor, 0, lagTime);

    for(int i=0; i<burstVectorDual.size(); ++i) {
        start=qMax(burstVectorDual.at(i).startIdxDonor-offset,0);
        end=qMin(start+burstVectorDual.at(i).n_Dem()+offset, arrivalTimeDonor.size());
        for(int j = start; j<end; ++j) gsl_histogram_increment (histDonor, arrivalTimeDonor[j]-arrivalTimeDonor[start]); // convert photon times to intensity

        start=qMax(burstVectorDual.at(i).startIdxAcceptor-offset,0);
        end=qMin(start+burstVectorDual.at(i).n_Aem()+offset,arrivalTimeAcceptor.size());
        for(int j = start; j<end; ++j) gsl_histogram_increment (histAcceptor, arrivalTimeAcceptor[j]-arrivalTimeAcceptor[start]);

        for (size_t i=0; i<histDonor->n; ++i) {
            corrD.correlate_step(histDonor->bin[i]);
            corrA.correlate_step(histAcceptor->bin[i]);
            ccorr.crosscorrelate_step(histDonor->bin[i], histAcceptor->bin[i]);
        }

        gsl_histogram_reset(histAcceptor);
        gsl_histogram_reset(histDonor);
    }

    corrD.normalize();
    corrA.normalize();
    ccorr.crossnormalize();


    // save correlation function in vectors
    unsigned int cN=corrD.getSlots();
    for (unsigned int i=0;i<cN;++i) tau.push_back(corrD.getCorTau()[i]*binwidth);
    for (unsigned int i=0;i<cN;++i) corrDonor.push_back(corrD.getCor()[i]);
    for (unsigned int i=0;i<cN;++i) corrAcceptor.push_back(corrA.getCor()[i]);
    for (unsigned int i=0;i<cN;++i) crossCorr.push_back(ccorr.getCor()[i]);

#ifdef MYDEBUG
    if ((cN==corrA.getSlots())&&(cN==ccorr.getSlots())) {
        qDebug("tau in ms\tDonor ACF\tAcceptor ACF\tCCF");
        for (unsigned int i=0; i<cN; ++i) {
            qDebug("%f\t%f\t%f\t%f", tau[i]*1e3, corrDonor[i],corrAcceptor[i],crossCorr[i]);
        }
    } else AlexEvalLog::warning("error");
#endif

    // free memory
    gsl_histogram_free(histDonor);
    gsl_histogram_free(histAcceptor);

    return EXIT_SUCCESS;
}


int oscillationACF_TTTR(QVector<double> &corrExc, QVector<double> &corrDonorExc, QVector<double> &corrAcceptorExc,const QVector<double> &tau, const FCSparams_t &FCSparams)
{
    qDebug("calculate ACF of excitation");
    if((corrDonorExc.size()>0)||(corrAcceptorExc.size()>0)||(corrExc.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        corrDonorExc.clear();
        corrAcceptorExc.clear();
        corrExc.clear();
    }
    if(tau.isEmpty()) {
        AlexEvalLog::warning("warning: no tau to correlate");
        return EXIT_FAILURE;
    }

    // parameter
//    int nPeriods=(int)(lagTime/binwidth+0.5);
    int nPeriods=FCSparams.nPeriods;
    double meanIPT=FCSparams.dummyPhotonIPT ;
    double sigma=meanIPT/3;

    int ntau=tau.size();
    corrExc.resize(ntau);
    corrDonorExc.resize(ntau);
    corrAcceptorExc.resize(ntau);

    QVector<double> marker;
    QVector<double> markerA;
    QVector<double> markerD;
    const gsl_rng_type * rngType;
    gsl_rng * r;

    rngType = gsl_rng_env_setup();
    r = gsl_rng_alloc(rngType);
    gsl_rng_set(r, 0); // set the seed to 0, i.e. the default value

    double Tend_donor=FCSparams.tDex;
    double Tend_pause1=Tend_donor+FCSparams.tGapDexAex;
    double Tend_acceptor=Tend_pause1+FCSparams.tAex;
    double Tend_pause2=Tend_acceptor+FCSparams.tGapAexDex;

//    nPeriods=qMax(FCSparams.BoxFilterIndexWindow*tau.last()/Tend_pause2,(double)nPeriods);
//    qDebug()<<"nPeriods="<<nPeriods<<"   "<<FCSparams.BoxFilterIndexWindow*tau.last()/Tend_pause2;
    qDebug()<<"nPeriods="<<nPeriods;
    int n=0;
    double t=0;
    while(n<nPeriods) {
        while((t<=(Tend_donor+n*Tend_pause2))) {
            markerD.append(t);
            t+=fabs(meanIPT+gsl_ran_gaussian(r,sigma));
        }
        while((t<=(Tend_pause1+n*Tend_pause2))) {
            t+=fabs(meanIPT+gsl_ran_gaussian(r,sigma));
        }
        while((t<=(Tend_acceptor+n*Tend_pause2))) {
            markerA.append(t);
            t+=fabs(meanIPT+gsl_ran_gaussian(r,sigma));
        }
        while((t<=(Tend_pause2+n*Tend_pause2))) {
            t+=fabs(meanIPT+gsl_ran_gaussian(r,sigma));
        }
        ++n;
    }


#ifdef MYDEBUG
    double binwidth=FCSparams.binwidth;
    nPeriods=100;
    int nbins=nPeriods*Tend_pause2/binwidth;
    gsl_histogram* hist=gsl_histogram_calloc_uniform(nbins,0,nPeriods*Tend_pause2);
    for(int i=0;markerD.at(i)<nPeriods*Tend_pause2;++i) {
        gsl_histogram_increment(hist,markerD.at(i));
    }

    qDebug()<<"time\tintensity";
    for(int i=0;i<nbins;++i) {
        qDebug()<<hist->range[i]<<"\t"<<hist->bin[i];
    }
#endif


    qDebug()<<"correlating"<<markerD.size()<<"marker events in Dex";

    correlateTTTR(markerD.constData(),markerD.size(),corrDonorExc.data(),tau.constData(),tau.size());
    correlateTTTR(markerA.constData(),markerA.size(),corrAcceptorExc.data(),tau.constData(),tau.size());

    corrExc=corrDonorExc;
//    mergeSorted(marker,markerD,markerA);
//    markerD.clear();markerA.clear();
//    correlateTTTR(marker.constData(),marker.size(),corrExc.data(),tau.constData(),tau.size());

    return EXIT_SUCCESS;
}


int oscillationACF(QVector<double> &corrExc, QVector<double> &corrDonorExc, QVector<double> &corrAcceptorExc, const FCSparams_t &FCSparams)
{
    qDebug("calculate ACF of excitation");
    if((corrDonorExc.size()>0)||(corrAcceptorExc.size()>0)||(corrExc.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        corrDonorExc.clear();
        corrAcceptorExc.clear();
        corrExc.clear();
    }

    // parameter
    // histogram (=intensity timetrace) of the alternating laser excitation scheme
    double Tend_donor=FCSparams.tDex;
    double Tend_pause1=Tend_donor+FCSparams.tGapDexAex;
    double Tend_acceptor=Tend_pause1+FCSparams.tAex;
    double Tend_pause2=Tend_acceptor+FCSparams.tGapAexDex;
    double binwidth=FCSparams.MinLagTime ;
    double lagTime=Tend_pause2*FCSparams.nPeriods;

    MultiTauCorrelator<double, double> corrOscD(FCSparams.S,FCSparams.m,FCSparams.P,binwidth);
    MultiTauCorrelator<double, double> corrOscA(FCSparams.S,FCSparams.m,FCSparams.P,binwidth);
    MultiTauCorrelator<double, double> corrOsc(FCSparams.S,FCSparams.m,FCSparams.P,binwidth);


    int n=0;
    double t=0;
    int nPeriods=(int)(lagTime/binwidth+0.5);
    while(n<nPeriods) {
        while((t<=(Tend_donor+n*Tend_pause2))) {
            corrOscD.correlate_step(1);
            corrOscA.correlate_step(0);
            corrOsc.correlate_step(1);
            t+=binwidth;
        }
        while((t<=(Tend_pause1+n*Tend_pause2))) {
            corrOscD.correlate_step(0);
            corrOscA.correlate_step(0);
            corrOsc.correlate_step(0);
            t+=binwidth;
        }
        while((t<=(Tend_acceptor+n*Tend_pause2))) {
            corrOscD.correlate_step(0);
            corrOscA.correlate_step(1);
            corrOsc.correlate_step(1);
            t+=binwidth;
        }
        while((t<=(Tend_pause2+n*Tend_pause2))) {
            corrOscD.correlate_step(0);
            corrOscA.correlate_step(0);
            corrOsc.correlate_step(0);
            t+=binwidth;
        }
        ++n;
    }


    corrOscD.normalize();
    corrOscA.normalize();
    corrOsc.normalize();

    for (unsigned int i=0;i<corrOsc.getSlots();++i) {
        corrDonorExc.push_back(corrOscD.getCor()[i]);
        corrAcceptorExc.push_back(corrOscA.getCor()[i]);
        corrExc.push_back(corrOsc.getCor()[i]);
    }

    return EXIT_SUCCESS;
}

int oscillationACF_OLD(QVector<double> &tau, QVector<double> &corrDonorExc, QVector<double> &corrAcceptorExc, const FCSparams_t &FCSparams)
{
    qDebug("calculate ACF of excitation");
    if((tau.size()>0)||(corrDonorExc.size()>0)||(corrAcceptorExc.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        tau.clear();
        corrDonorExc.clear();
        corrAcceptorExc.clear();
    }

    // parameter
    // histogram (=intensity timetrace) of the alternating laser excitation scheme
    double Tend_donor=FCSparams.tDex;
    double Tend_pause1=FCSparams.tDex+FCSparams.tGapDexAex;
    double Tend_acceptor=FCSparams.tDex+FCSparams.tGapDexAex+FCSparams.tAex;
    double Tend_pause2=FCSparams.tDex+FCSparams.tGapDexAex+FCSparams.tAex+FCSparams.tGapAexDex;
    double binwidth=FCSparams.MinLagTime ;
    double lagTime=Tend_pause2*FCSparams.nPeriods;

    int nbins=(int)(lagTime/binwidth+0.5);
    MultiTauCorrelator<double, double> corrOscD(FCSparams.S,FCSparams.m,FCSparams.P,1);
    MultiTauCorrelator<double, double> corrOscA(FCSparams.S,FCSparams.m,FCSparams.P,1);


    gsl_histogram * histOscDonor = gsl_histogram_alloc(nbins); // oscillation through laser alternation
    gsl_histogram * histOscAcceptor = gsl_histogram_alloc(nbins); // oscillation through laser alternation
    gsl_histogram_set_ranges_uniform (histOscDonor, 0, lagTime);
    gsl_histogram_set_ranges_uniform (histOscAcceptor, 0, lagTime);

    size_t i=0;
    double n=0;
    while(i<histOscDonor->n) {
        while((histOscDonor->range[i]<=(Tend_donor+n*Tend_pause2)) && i<histOscDonor->n) { //while(histOsc->range[i]<T1) histOsc->bin[i++]++;
            histOscDonor->bin[i]=1;
            histOscAcceptor->bin[i]=0;
            i++;
        }
        while((histOscDonor->range[i]<=(Tend_pause1+n*Tend_pause2)) && i<histOscDonor->n) {
            histOscDonor->bin[i]=0;
            histOscAcceptor->bin[i]=0;
            i++;
        }
        while((histOscDonor->range[i]<=(Tend_acceptor+n*Tend_pause2)) && i<histOscDonor->n) {
            histOscDonor->bin[i]=0;
            histOscAcceptor->bin[i]=1;
            i++;
        }
        while((histOscDonor->range[i]<=(Tend_pause2+n*Tend_pause2)) && i<histOscDonor->n) {
            histOscDonor->bin[i]=0;
            histOscAcceptor->bin[i]=0;
            i++;
        }
        n++;
    }
    double varOsc=0; // FIXME
    for (size_t k=0; k<histOscDonor->n; ++k) {
        corrOscD.correlate_step(histOscDonor->bin[k]);
        varOsc+=histOscDonor->bin[k]*histOscDonor->bin[k];
//        qDebug()<<histOscDonor->range[k]*1e6<<"\t"<<histOscDonor->bin[k];
    }
    varOsc=varOsc/histOscDonor->n;
    qDebug()<<"varOsc="<<varOsc;

//    corrOscD.correlate_series(histOscDonor->bin,histOscDonor->n);
//    corrOscA.correlate_series(histOscAcceptor->bin,histOscAcceptor->n);
    corrOscA.correlate_series(histOscAcceptor->bin,histOscAcceptor->n);
    corrOscD.normalize();
    corrOscA.normalize();

    for (unsigned int i=0;i<corrOscD.getSlots();++i) {
        corrDonorExc.push_back(corrOscD.getCor()[i]);
        corrAcceptorExc.push_back(corrOscA.getCor()[i]);
        tau.push_back(corrOscD.getCorTau()[i]*binwidth);
    }

    return EXIT_SUCCESS;
}


int selectSubpopulation(QVector<BurstDual> &burstVectorDualSelected, const QVector<BurstDual> &burstVectorDual, const FRETparams_t &FRETparams)
{
    qDebug("selectSubpopulation");
    if(burstVectorDual.isEmpty()) {
        AlexEvalLog::warning("no bursts to select");
        return EXIT_FAILURE;
    }
    if (burstVectorDualSelected.size()>0) {
        AlexEvalLog::warning("overwriting selected subpopulation");
        burstVectorDualSelected.clear();
    }

    for(QVector<BurstDual>::const_iterator burst=burstVectorDual.begin(); burst!=burstVectorDual.end();++burst) {
        if(burst->isMasked(FRETparams)); // exclude burst
        else burstVectorDualSelected.append(*burst);
    }

    if (burstVectorDualSelected.isEmpty()) {
        AlexEvalLog::warning("no bursts found matching selection criteria");
        return EXIT_FAILURE;
    } else {
        AlexEvalLog::text("selected " + QString::number(burstVectorDualSelected.size()) + " bursts");
    }
    return EXIT_SUCCESS;
}



int boxFilter(QVector<double> &filteredData, const QVector<double> &xdata, const QVector<double> &ydata, const double &rangeWindow)
{
    int nMax=qMin(xdata.size(),ydata.size());
    if(filteredData.size()>0) {
        AlexEvalLog::warning("boxFilter: overwriting data");
        filteredData.clear();
    }
    // box filter
    double c;
    int start,stop;
    for (int i=0;i<nMax;++i) {
        // search for start and stop indices inside the time window
        start=i; stop=i;
        while(start>0 && (xdata.at(start) + rangeWindow) > xdata.at(i) ) start--;
        while(stop<nMax-1  && (xdata.at(stop) - rangeWindow) < xdata.at(i) ) stop++;
//        qDebug()<<"start="<<start<<"stop="<<stop<<"at i="<<i;
        // calculate boxed mean
        c=0;
        for(int j=start;j<=stop;++j) c+= ydata.at(j);
        c/=(stop-start+1);
        filteredData.push_back(c);
    }

    return EXIT_SUCCESS;
}



int boxFilter(QVector<double> &filteredData, const QVector<double> &data, const int &indexWindow)
{
    if(filteredData.size()>0) {
        AlexEvalLog::warning("boxFilter: overwriting data");
        filteredData.clear();
    }
    // box filter
    double c;
    int start,stop;
    for (int i=0;i<data.size();++i) {
        // calculate start and stop indices inside the time window
        start=qMax(i-indexWindow,0);
        stop=qMin(i+indexWindow,data.size()-1);
//        qDebug()<<"start="<<start<<"stop="<<stop<<"at i="<<i;
        // calculate boxed mean
        c=0;
        for(int j=start;j<=stop;++j) c+= data.at(j);
        c/=(stop-start+1);
        filteredData.push_back(c);
    }
    return EXIT_SUCCESS;
}


// NEW
#ifdef TIMER
#include "ticktock.h"
#endif
int crosscorrelateTTTR(const double *t, uint Nt, const double *u, uint Nu, double *g, const double *tau, const uint ntau)
{
#ifdef TIMER
    PublicTickTock timer;
    timer.tick();
#endif
    double tDeltaStart, tDeltaEnd;
    double tStart=qMin(t[0],u[0]);
    double tEnd = qMax(t[Nt-1],u[Nu-1]);
    double T = tEnd - tStart; // total acquisition time
    double *lk=new double[ntau]; // photon indices
    double *mk=new double[ntau]; // photon indices
    double *nt=new double[ntau]; // photon count in channel t
    double *nu=new double[ntau]; // photon count in channel u

    //    (1) Initialize a correlogram Yk with M bins to 0.
    for(uint k=0;k<ntau;++k) {
        g[k]=0;
        lk[k]=1;
        mk[k]=1;
        nt[k]=0;
        nu[k]=0;
    }
#ifdef TIMER
    timer.tockPrint("init");
    timer.tick();
#endif


    for(uint i=0;i<Nt;++i) {// loop over all photons in channel t

        // the "0th" bin
        uint l=lk[0];
        while(l<(Nu-1) && (u[l] < t[i] ) ) ++l;
        lk[0]=l;
        tDeltaEnd = t[i] + tau[0];
        uint m=mk[0];
        while(m<(Nu-1) && (u[m] < tDeltaEnd) ) ++m;
        mk[0] = m;
        g[0] = g[0] + mk[0] - lk[0];
        if((t[i]-tStart)<=(T-tau[0])) ++nt[0];

//        qDebug()<<lk[0]<<mk[0]<<t[i]<<u[l]<<u[m]<<g[0]<<l<<m;

        for(uint k=1;k<ntau;++k) { // loop over bins k>0

            tDeltaStart = t[i] + tau[k-1];

            l=lk[k];
            while(l<Nu && u[l]<tDeltaStart) ++l;
            lk[k]=l;

            tDeltaEnd = t[i] + tau[k];

            m=mk[k];
            while(m<Nu && u[m]<tDeltaEnd) ++m;
            mk[k]=m;

            g[k] = g[k] + mk[k] - lk[k];  // update correlogram

            if((t[i]-tStart)<=(T-tau[k])) ++nt[k];
        }
    }
#ifdef TIMER
    timer.tockPrint("correlation");
    timer.tick();
#endif
    // Normalization for channel u
    for(uint j=0;j<Nu;++j) {// loop over all photons in channel u
        for(uint k=0;k<ntau;++k) { // loop over bins
            if((u[j]-tStart)>=tau[k]) ++nu[k];
        }
    }
    // normalize correlogram
    g[0]=g[0]*(T-tau[0])/(tau[0])/(nt[0]*nu[0]);
    for(uint k=1;k<ntau;++k) {
        // normalize by the number of photons for the lag times (symmetrically for u and t)
        // and the binwidth tau_{k} - tau_{k-1} (rectangular averaging).
        g[k]=g[k]*(T-tau[k])/(tau[k]-tau[k-1])/(nt[k]*nu[k]);
    }
#ifdef TIMER
    timer.tockPrint("norm");
#endif

    delete [] nt;
    delete [] nu;
    delete [] lk;
    delete [] mk;


    return EXIT_SUCCESS;
}


// _NEW
int correlateTTTR(const double *t, const uint N, double *g, const double *tau, const uint ntau)
{
    double tDeltaStart, tDeltaEnd;
    double tStart=t[0]; // assuming measurement begins at t[0]
    double T = t[N-1]-t[0]; // total acquisition time assuming measurement begins at t[0]
    const double *u=t;
    double *lk=new double[ntau]; // photon indices
    double *mk=new double[ntau]; // photon indices
    double *nt=new double[ntau]; // photon count in channel t
    double *nu=new double[ntau]; // photon count in channel u

    //    (1) Initialize a correlogram Yk with M bins to 0.
    for(uint k=0;k<ntau;++k) {
        g[k]=0;
        lk[k]=1;
        mk[k]=1;
        nt[k]=0;
        nu[k]=0;
    }


    for(uint i=0;i<N;++i) {// loop over all photons in channel t

        // the "0th" bin
        lk[0] = i; // for autocorrelation, the indices match
        uint l=lk[0];
        tDeltaEnd = t[i] + tau[0];
        uint m=mk[0];
        if(m==0) m=1;
        while((u[m-1] >= tDeltaEnd || tDeltaEnd > u[m]) && m<(N-1)) ++m;
        mk[0] = m;
        g[0] = g[0] + mk[0] - lk[0];
        if((t[i]-tStart)<=(T-tau[0])) ++nt[0];
        if((u[i]-tStart)>=tau[0]) ++nu[0];


        for(uint k=1;k<ntau;++k) { // loop over bins k>0

            tDeltaStart = t[i] + tau[k-1];

            l=lk[k];
            while(l<N && u[l]<tDeltaStart) ++l;
            lk[k]=l;

            tDeltaEnd = t[i] + tau[k];

            m=mk[k];
            while(m<N && u[m]<tDeltaEnd) ++m;
            mk[k]=m;

//            int m=mk[k]; while((u[m-1] >= tauEnd || tauEnd > u[m]) || m==N) m++; mk[k] = m;

            g[k] = g[k] + mk[k] - lk[k];  // update correlogram

            if((t[i]-tStart)<=(T-tau[k])) ++nt[k];
            if((u[i]-tStart)>=tau[k]) ++nu[k];
        }
    }
    // Normalize
//    qDebug()<<"normalization k g[k]\ttau[k]\tnt[k]\tnu[k]\n";
    g[0]=g[0]*(T-tau[0])/(tau[0])/(nt[0]*nu[0]);
//    qDebug()<<0<<"\t"<<g[0]<<"\t"<<tau[0]<<"\t"<<nt[0]<<"\t"<<nu[0]<<"\n";
    for(uint k=1;k<ntau;++k) {
        // normalize by the number of photons for the lag times (symmetrically for u and t)
        // and the binwidth tau_{k} - tau_{k-1} (rectangular averaging).
//        qDebug()<<k<<"\t"<<g[k]<<"\t"<<tau[k]<<"\t"<<nt[k]<<"\t"<<nu[k]<<"\n";
        g[k]=g[k]*(T-tau[k])/(tau[k]-tau[k-1])/(nt[k]*nu[k]);
    }

    delete [] nt;
    delete [] nu;
    delete [] lk;
    delete [] mk;


    return EXIT_SUCCESS;
}


int crosscorrelateTTTR_OLD(const double *t, uint Nt, const double *u, uint Nu, double *g, double *tau, uint ntau)
{
    double tauStart, tauEnd;
    double tStart=qMin(t[0],u[0]);
    double tEnd = qMax(t[Nt-1],u[Nu-1]);
    double T = tEnd - tStart; // total acquisition time
    double *lk=new double[ntau]; // photon indices
    double *mk=new double[ntau]; // photon indices
    double *nt=new double[ntau]; // photon count in channel t
    double *nu=new double[ntau]; // photon count in channel u

    //    (1) Initialize a correlogram Yk with M bins to 0.
    for(uint k=0;k<ntau;++k) {
        g[k]=0;
        lk[k]=1;
        mk[k]=1;
        nt[k]=0;
        nu[k]=0;
    }


    for(uint i=0;i<Nt;++i) {// loop over all photons in channel t

        for(uint k=0;k<ntau-1;++k) { // loop over bins

            tauStart = t[i] + tau[k];
            for(uint l=lk[k]; l<Nu; ++l) { // start photon in bin k
                if ((u[l-1] < tauStart && tauStart <= u[l]) ) { //|| l==N
                    lk[k] = l;
                    break;
                }
            }

            tauEnd = t[i] + tau[k+1];
            for(uint m=mk[k]; m<Nu; ++m) { // end photon in bin k
                if ( (u[m-1] < tauEnd && tauEnd <= u[m])) { // || m==N
                    mk[k] = m;
                    break;
                }
            }
//            int m=mk[k]; while((u[m-1] >= tauEnd || tauEnd > u[m]) || m==N) m++; mk[k] = m;

            g[k] = g[k] + mk[k] - lk[k];  // update correlogram

            if((t[i]-tStart)<=(T-tau[k])) ++nt[k];
        }
    }

    // Normalize
    for(uint i=0;i<Nu;++i) {// loop over all photons in channel u
        for(uint k=0;k<ntau-1;++k) { // loop over bins
            if((u[i]-tStart)>=tau[k]) ++nu[k];
        }
    }
    for(uint k=0;k<ntau-1;++k) {
        // normalize by the number of photons for the lag times (symmetrically for u and t)
        // and the binwidth tau_{k+1} - tau_k (rectangular averaging).
        g[k]=g[k]*(T-tau[k])/(tau[k+1]-tau[k])/(nt[k]*nu[k]);
    }

    delete [] nt;
    delete [] nu;
    delete [] lk;
    delete [] mk;


    return EXIT_SUCCESS;
}



/* OLD INDICES */
int correlateTTTR_OLD(const double *t, const uint N, double *g, const double *tau, const uint ntau)
{
    double tauStart, tauEnd;
    double T = t[N-1]-t[0]; // total acquisition time assuming measurement begins at t[0]
    const double *u=t;
    double *lk=new double[ntau]; // photon indices
    double *mk=new double[ntau]; // photon indices
    double *nt=new double[ntau]; // photon count in channel t
    double *nu=new double[ntau]; // photon count in channel u

    //    (1) Initialize a correlogram Yk with M bins to 0.
    for(uint k=0;k<ntau;++k) {
        g[k]=0;
        lk[k]=1;
        mk[k]=1;
        nt[k]=0;
        nu[k]=0;
    }


    for(uint i=0;i<N;++i) {// loop over all photons in channel t

        for(uint k=0;k<ntau-1;++k) { // loop over bins

            tauStart = t[i] + tau[k];

            for(uint l=lk[k]; l<N; ++l) { // start photon in bin k
                if ((u[l-1] < tauStart && tauStart <= u[l]) || l==N ) {
                    lk[k] = l;
                    break;
                }
            }

            tauEnd = t[i] + tau[k+1];

            for(uint m=mk[k]; m<N; ++m) { // end photon in bin k
                if ( (u[m-1] < tauEnd && tauEnd <= u[m]) || m==N ) {
                    mk[k] = m;
                    break;
                }
            }
//            int m=mk[k]; while((u[m-1] >= tauEnd || tauEnd > u[m]) && m<N) m++; mk[k] = m;

            g[k] = g[k] + mk[k] - lk[k];  // update correlogram

            if((t[i]-t[0])<=(T-tau[k])) ++nt[k]; // assuming measurement begins at t[0]
            if((u[i]-t[0])>=tau[k]) ++nu[k];
        }
    }
    // Normalize
//    qDebug()<<"normalization k g[k]\ttau[k]\tnt[k]\tnu[k]\n";
    for(uint k=0;k<ntau-1;++k) {
        // normalize by the number of photons for the lag times (symmetrically for u and t)
        // and the binwidth tau_{k+1} - tau_k (rectangular averaging).
//        qDebug()<<k<<"\t"<<g[k]<<"\t"<<tau[k]<<"\t"<<nt[k]<<"\t"<<nu[k]<<"\n";
        g[k]=g[k]*(T-tau[k])/(tau[k+1]-tau[k])/(nt[k]*nu[k]);
    }

    delete [] nt;
    delete [] nu;
    delete [] lk;
    delete [] mk;


    return EXIT_SUCCESS;
}
//*/



int correlateTOA(QVector<double> &tau, QVector<double> &corr,const double *arrivalTime, const uint N, const FCSparams_t &FCSparams)
{
    uint ntau;
    if(tau.isEmpty()) {
        // parameter
        int S=FCSparams.S; // blocks
        int m=FCSparams.m; // multiply delta tau
        int P=FCSparams.P; // tau values per block

        ntau=P+(S-1)*(P-P/m);
        double*tauIdx = new double[ntau];
        MultiTauCorrelatorCreateTau(tauIdx,S,m,P); // create log-spaced tau values in array tau. note that the values refer to the index in the array to correlate, i.e. in this case to the number of bins.
        for(uint i=0;i<ntau;++i) tau.push_back(tauIdx[i]*FCSparams.MinLagTime);
    } else {
        ntau=tau.size();
    }

    corr.resize(tau.size());

    return correlateTTTR(arrivalTime,N,corr.data(),tau.data(),ntau);

}



int crossCorrelateTOA(QVector<double> &tau, QVector<double> &corr, const double *arrivalTime1, const uint N1, const double *arrivalTime2, const uint N2, const FCSparams_t &FCSparams)
{
    uint32_t ntau=0;
    if(tau.isEmpty()) {
        // parameter
        int S=FCSparams.S; // blocks
        int m=FCSparams.m; // multiply delta tau
        int P=FCSparams.P; // tau values per block

        ntau=P+(S-1)*(P-P/m);
        double*tauIdx = new double[ntau];
        MultiTauCorrelatorCreateTau(tauIdx,S,m,P); // create log-spaced tau values in array tau. note that the values refer to the index in the array to correlate, i.e. in this case to the number of bins.
        for(uint i=0;i<ntau;++i) tau.append(tauIdx[i]*FCSparams.MinLagTime);
    } else {
        ntau=tau.size();
    }
    corr.resize(tau.size());

    return crosscorrelateTTTR(arrivalTime1,N1,arrivalTime2,N2,corr.data(),tau.data(),ntau);
}



int testCorrelator(QVector<double> &tau, QVector<double> &corr,const QVector<double> &arrivalTime, const FCSparams_t &FCSparams)
{
    uint32_t ntau=0;
    if(tau.isEmpty()) {
        // parameter
        int S=FCSparams.S; // blocks
        int m=FCSparams.m; // multiply delta tau
        int P=FCSparams.P; // tau values per block

        ntau=P+(S-1)*(P-P/m);
        double*tauIdx = new double[ntau];
        MultiTauCorrelatorCreateTau(tauIdx,S,m,P); // create log-spaced tau values in array tau. note that the values refer to the index in the array to correlate, i.e. in this case to the number of bins.
        for(uint i=0;i<ntau;++i) tau.append(tauIdx[i]*FCSparams.MinLagTime);
    } else {
        ntau=tau.size();
    }
    corr.resize(tau.size());

    correlateTTTR(arrivalTime.constData(),arrivalTime.size(),corr.data(),tau.data(),ntau);

    QVector<double> corrOLD(tau.size());
    correlateTTTR_OLD(arrivalTime.constData(),arrivalTime.size(),corrOLD.data(),tau.data(),ntau);

    QVector<double> ccorrOLD(tau.size());
    crosscorrelateTTTR(arrivalTime.constData(),arrivalTime.size(),arrivalTime.constData(),arrivalTime.size(),ccorrOLD.data(),tau.data(),ntau);

    qDebug()<<"tau\tcorr\tcorrOLD\tccorrOLD";
    for(int i =0; i<tau.size();++i) {
        qDebug()<<tau.at(i)<<"\t"<<corr.at(i)<<"\t"<<corrOLD.at(i)<<"\t"<<ccorrOLD.at(i);
    }

    return EXIT_SUCCESS;
}



int FCS_TTTR(QVector<double> &tau, QVector<double> &corrDonor, QVector<double> &corrAcceptor, QVector<double> &crossCorr, const QVector<double> &arrivalTimeDemDex, const QVector<double> &arrivalTimeAemDex, const FCSparams_t &FCSparams)
{
    uint32_t ntau=0;
    if(tau.isEmpty()) {
        // parameter
        int S=FCSparams.S; // blocks
        int m=FCSparams.m; // multiply delta tau
        int P=FCSparams.P; // tau values per block

        ntau=P+(S-1)*(P-P/m);
        tau.resize(ntau);
        MultiTauCorrelatorCreateTau(tau.data(),S,m,P); // create log-spaced tau values in array tau. note that the values refer to the index in the array to correlate, i.e. in this case to the number of bins.
        for(uint i=0;i<ntau;++i) tau[i]*=FCSparams.MinLagTime;
    } else {
        ntau=tau.size();
    }
    corrDonor.resize(ntau);
    corrAcceptor.resize(ntau);
    crossCorr.resize(ntau);

    // filter photon arrival times
    // take only photons during donor excitation and within burst + offset;
    // FIXME: offset not included
    int res;
    res =correlateTTTR(arrivalTimeDemDex.constData(),arrivalTimeDemDex.size(),corrDonor.data(),tau.data(),ntau);
    res+=correlateTTTR(arrivalTimeAemDex.constData(),arrivalTimeAemDex.size(),corrAcceptor.data(),tau.data(),ntau);
    res+=crosscorrelateTTTR(arrivalTimeDemDex.constData(),arrivalTimeDemDex.size(),arrivalTimeAemDex.constData(),arrivalTimeAemDex.size(),crossCorr.data(),tau.data(),ntau);

    return res;
}



int FCS_TTTR_OLD(QVector<double> &tau, QVector<double> &corrDonor, QVector<double> &corrAcceptor, QVector<double> &crossCorr, const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor, const QVector<double> markerTimeDexc, const QVector<double> markerTimeAexc, const QVector<BurstDual> &burstVectorDual, const FCSparams_t &FCSparams)
{
    uint32_t ntau=0;
    if(tau.isEmpty()) {
        // parameter
        int S=FCSparams.S; // blocks
        int m=FCSparams.m; // multiply delta tau
        int P=FCSparams.P; // tau values per block

        ntau=P+(S-1)*(P-P/m);
        double*tauIdx = new double[ntau];
        MultiTauCorrelatorCreateTau(tauIdx,S,m,P); // create log-spaced tau values in array tau. note that the values refer to the index in the array to correlate, i.e. in this case to the number of bins.
        for(uint i=0;i<ntau;++i) tau.append(tauIdx[i]*FCSparams.MinLagTime);
    } else {
        ntau=tau.size();
    }
    corrDonor.resize(ntau);
    corrAcceptor.resize(ntau);
    crossCorr.resize(ntau);

    // filter photon arrival times
    // take only photons during donor excitation and within burst + offset;
    // FIXME: offset not included
    QVector<double> TOAfilteredD;
    QVector<double> TOAfilteredA;

    int markerIdx=0,lastD=0,lastA=0;

    for(QVector<BurstDual>::const_iterator burst=burstVectorDual.begin(); burst!=burstVectorDual.end();++burst) {
        while(markerTimeDexc.at(markerIdx)<burst->startTime) ++markerIdx;
        --markerIdx;

        while(markerTimeDexc.at(markerIdx)<burst->endTime()) {
            while(arrivalTimeDonor.at(lastD)<markerTimeDexc.at(markerIdx)) ++lastD; // advance to begin of Dexc
            while(arrivalTimeDonor.at(lastD)<markerTimeAexc.at(markerIdx)) { // count photons to the begin of Aex (= end of Dex)
                TOAfilteredD.append(arrivalTimeDonor.at(lastD));
                ++lastD;
            }
            while(arrivalTimeAcceptor.at(lastA)<markerTimeDexc.at(markerIdx)) ++lastA;
            while(arrivalTimeAcceptor.at(lastA)<markerTimeAexc.at(markerIdx)) {
                TOAfilteredA.append(arrivalTimeAcceptor.at(lastA));
                ++lastA;
            }
            ++markerIdx;
        }
    }

    int res;
    res =correlateTTTR(TOAfilteredD.constData(),TOAfilteredD.size(),corrDonor.data(),tau.data(),ntau);
    res+=correlateTTTR(TOAfilteredA.constData(),TOAfilteredA.size(),corrAcceptor.data(),tau.data(),ntau);
    res+=crosscorrelateTTTR(TOAfilteredD.constData(),TOAfilteredD.size(),TOAfilteredA.constData(),TOAfilteredA.size(),crossCorr.data(),tau.data(),ntau);

//    // alternative: lag time tau + N*T is only correlation because of exc.
//    QVector<double> tauOsc;
//    for(int i=0;i<fd->tau.size();++i) tauOsc.append(fd->tau.at(i)+ad->FCSparams.MaxLagTime);
//    fd->ACF_Dem_Dex_later.resize(tauOsc.size());
//    res+=correlateTTTR(TOAfilteredA.constData(),TOAfilteredA.size(),fd->ACF_Dem_Dex_later.data(),tauOsc.data(),ntau);

    return res;
}



void MultiTauCorrelatorCreateTau(double *tau, int S, int m, int P, double tauMin)
{
    double *deltaTau=(double*)calloc(S, sizeof(double));
    deltaTau[0]=tauMin;

    for (int s=1; s<S; s++) {
        deltaTau[s]=deltaTau[s-1]*(double)m;
    }
//    unsigned int corr_slots=P+(S-1)*(P-P/m);
//    tau=(double*)calloc(corr_slots, sizeof(double));
    tau[0]=tauMin;
    for (int k=1; k<P; k++) {
        tau[k]=tau[k-1]+deltaTau[0];
    }
    int k=P;
    for (int s=1; s<S; s++) {
        for ( int i=0; i<P-P/m; i++) {
            tau[k]=tau[k-1]+deltaTau[s];
            k++;
        }
    }
    delete [] deltaTau;
}



int mergeSorted(QVector<double> &mergedVector, const QVector<double> &orderedVector1, const QVector<double> &orderedVector2)
{
    if (!mergedVector.isEmpty()) {
        qDebug()<<"warning: overwriting data";
    }
    int size=orderedVector1.size()+orderedVector2.size();
    mergedVector.resize(size);

    int idx1=0,idx2=0,idxMerged=0;
    while(idx1<orderedVector1.size() && (idx2<orderedVector2.size())) {
        if(orderedVector1.at(idx1)<orderedVector2.at(idx2)) {
            mergedVector[idxMerged]=orderedVector1.at(idx1);
            ++idx1;
        } else {
            mergedVector[idxMerged]=orderedVector2.at(idx2);
            ++idx2;
        }
        ++idxMerged;
    }
    while(idx1<orderedVector1.size()) {
        mergedVector[idxMerged]=orderedVector1.at(idx1);
        ++idx1;
        ++idxMerged;
    }
    while(idx2<orderedVector2.size()) {
        mergedVector[idxMerged]=orderedVector2.at(idx2);
        ++idx2;
        ++idxMerged;
    }
    return EXIT_SUCCESS;

}



void createLogTauTTTR(double *tau, double a, double b, unsigned int n)
{
    double step = (b-a)/(double)(n-1);
    for(unsigned int i=0;i<n;++i) {
        tau[i]=pow(10,a+step*i);
    }
}


void createLinspaceTau(double *tau, double a, double b, unsigned int n)
{
    double step = (b-a)/(double)(n-1);
    for(unsigned int i=0;i<n;++i) {
        tau[i]=a+step*i;
    }
}
