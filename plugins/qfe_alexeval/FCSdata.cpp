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

#include "FCSdata.h"
#include <QFile>

FCSdata::FCSdata()
{
}

QVector<double> FCSdata::meanTau() const
{
    QVector<double> meanTau;
    if(tau.isEmpty()) return meanTau;
    meanTau.append(tau.at(0)/2); //(tau.at(0)-0)/2
    for(int i=1;i<tau.size();++i) {
        meanTau.append((tau.at(i)+tau.at(i-1))/2);
    }
    return meanTau;
}

bool FCSdata::isEmpty() const
{
    return((tau.isEmpty())||(ACF_Dem_Dex.isEmpty())||(ACF_Aem_Dex.isEmpty())||(CCF_Dex.isEmpty()));
}

void FCSdata::saveCorr(const QString &filename, const FCSparams_t FCSparams) const
{
    QFile file;
    if(filename.isEmpty()) qFatal("FCSdata::saveCorr: enter a valid filename");
    else file.setFileName(filename);

    if(tau.size()==0) AlexEvalLog::warning("vector is empty");

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) AlexEvalLog::warning(file.errorString());
    QTextStream out(&file);
    QString FCSstr(FCSparams.toString());
    out <<"# "<<FCSstr.replace("\n","\t")<<"\n";
    out.setRealNumberPrecision(11);
    QVector<double> temp;
    if(FCSparams.doCenterTau) {
        temp=meanTau();
        out <<"# centered tau in s\tACF_Dem_Dex\tACF_Aem_Dex\tCCF_DA_Dex\tSUM_AD_Dex\t\n";
    } else {
        temp=tau;
        out <<"# tau in s\tACF_Dem_Dex\tACF_Aem_Dex\tCCF_DA_Dex\tSUM_AD_Dex\t\n";
    }
    for(int i=0;i<tau.size();++i) {
        out <<temp.at(i)<<","<<CDoubleToQString(ACF_Dem_Dex.at(i))<<","
           <<CDoubleToQString(ACF_Aem_Dex.at(i))<<","<<CDoubleToQString(CCF_Dex.at(i))<<","<<CDoubleToQString(ACF_sumAD_Dex.at(i))<<"\n";
    }
    AlexEvalLog::text(tau.size()+"correlation points written to file "+file.fileName());
    file.close();
}

QString FCSdata::getCorrCSV(int which, const FCSparams_t FCSparams) const
{
    QString file;

    if(tau.size()==0) AlexEvalLog::warning("vector is empty");

    QTextStream out(&file);
    QString FCSstr(FCSparams.toString());
    out <<"# "<<FCSstr.replace("\n","\t")<<"\n";
    out.setRealNumberPrecision(11);
    QVector<double> temp;
    if(FCSparams.doCenterTau) {
        temp=meanTau();
        out <<"# centered tau in s\tACF_Dem_Dex\tACF_Aem_Dex\tCCF_DA_Dex\tSUM_AD_Dex\t\n";
    } else {
        temp=tau;
        out <<"# tau in s\tACF_Dem_Dex\tACF_Aem_Dex\tCCF_DA_Dex\tSUM_AD_Dex\t\n";
    }
    for(int i=0;i<tau.size();++i) {
        if (which==0) {
            out <<temp.at(i)<<","<<CDoubleToQString(ACF_Dem_Dex.at(i))<<"\n";
        } else if (which==1) {
            out <<temp.at(i)<<","<<CDoubleToQString(ACF_Aem_Dex.at(i))<<"\n";
        } else if (which==2) {
            out <<temp.at(i)<<","<<CDoubleToQString(CCF_Dex.at(i))<<"\n";
        } else if (which==3) {
            out <<temp.at(i)<<","<<CDoubleToQString(ACF_sumAD_Dex.at(i))<<"\n";
        }

    }
    return file;
}

QString FCSdata::resultFCS(const QString prefix) const
{
    QString str = prefix + "#bursts in subpopulation=" + QString::number(subpopulation.size()) + "\n";
    if(tau.size()>0) {
        str += "tau_min=" + QString::number(tau.first(),'g',2) + "\n"
                "tau_max=" + QString::number(tau.last(),'g',2);
    }
    return str;
}

void FCSdata::clear()
{
    tau.clear();
    subpopulation.clear();
    ACF_Dem_Dex.clear();
    ACF_Aem_Dex.clear();
    ACF_Aem_Aex.clear();
    ACF_Aem.clear();
    ACF_Dem.clear();
    ACF_Dem_Dex_later.clear();
    ACF_sumAD_Dex.clear();
    CCF_Dex.clear();
}

void FCSdata::clearMarker()
{
    AlexEvalLog::text("clearing marker ACF...");
    ACF_marker.clear();
    ACF_markerD.clear();
    ACF_markerA.clear();
}

void FCSdata::remove(int index)
{
    tau.remove(index);
    ACF_Dem_Dex.remove(index);
    ACF_Aem_Dex.remove(index);
    ACF_Aem_Aex.remove(index);
    ACF_Aem.remove(index);
    ACF_Dem.remove(index);
    ACF_Dem_Dex_later.remove(index);
    ACF_sumAD_Dex.remove(index);
    CCF_Dex.remove(index);
    if(ACF_markerD.size()==tau.size()-1) {
        ACF_marker.remove(index);
        ACF_markerD.remove(index);
        ACF_markerA.remove(index);
    }
}




gsl_histogram *FCSdata::burstDurationDistribution(size_t nbins, BurstSearchParams_t burstSearchParams)
{
    gsl_histogram *hist =  gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform(hist,0,burstSearchParams.MaxDuration*1e3);

    for(int i=0;i<subpopulation.size();++i)
        gsl_histogram_increment(hist, subpopulation.at(i).duration*1e3);

    return hist;
}

gsl_histogram *FCSdata::burstSizeDistribution(size_t nbins, BurstSearchParams_t burstSearchParams)
{
    gsl_histogram *hist =  gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform(hist,0,burstSearchParams.MaxBurstrate*burstSearchParams.MaxDuration);

    for(int i=0;i<subpopulation.size();++i)
        gsl_histogram_increment(hist, subpopulation.at(i).numberOfPhotons);

    return hist;
}

gsl_histogram *FCSdata::burstRateDistribution(size_t nbins, BurstSearchParams_t burstSearchParams)
{
    gsl_histogram *hist =  gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform(hist,burstSearchParams.MinBurstrate*1e-3,burstSearchParams.MaxBurstrate*1e-3);

    for(int i=0;i<subpopulation.size();++i)
        gsl_histogram_increment(hist, subpopulation.at(i).burstrate()*1e-3);

    return hist;
}




int analysisFCStotal(FCSdata *fd, const AlexData *ad)
{
    int res=EXIT_FAILURE;
    fd->clear();

    PublicTickTock t;

    if(ad->FCSparams.FCSMethod==FCSparams_t::FCScomplete) {
        res=FCSmultiTauBinned(ad->arrivalTimeCh1,ad->arrivalTimeCh2, ad->FCSparams);
        analysisFCS(fd,ad);
        return res;
    }
    t.tick();
    if(selectSubpopulation(fd->subpopulation,ad->burstVectorDual,ad->FRETparams)==EXIT_FAILURE) return EXIT_FAILURE;
    t.tockPrint("selectSubpopulation");

    t.tick();
    QVector<double> TOA_Dex,TOA_Aem,TOA_Dem;
//         QVector<double> markerD,markerA,markerDA;
    QVector<double> TOA_Aem_Aex,TOA_Dem_Dex,TOA_Aem_Dex;

    if(burstPhotonsToChannel(TOA_Dem_Dex,TOA_Aem_Dex,TOA_Aem_Aex,ad->photons, fd->subpopulation, ad->FCSparams)) return EXIT_FAILURE;
    mergeSorted(TOA_Dex,TOA_Dem_Dex,TOA_Aem_Dex);
    mergeSorted(TOA_Aem,TOA_Aem_Aex,TOA_Aem_Dex);
    t.tockPrint("vector allocation");

    if(ad->FCSparams.FCSMethod==FCSparams_t::FCSstep) {
//        if(selectSubpopulation(fd->subpopulation,ad->burstVectorDual,ad->FRETparams)==EXIT_FAILURE) return EXIT_FAILURE;
//        ad->arrivalTimeCh1.clear();
//        ad->arrivalTimeCh2.clear();
//        ad->photons.vectorChannelMarker(ad->arrivalTimeCh1,DonorEx|DonorEm);
//        ad->photons.vectorChannelMarker(ad->arrivalTimeCh2,DonorEx|AcceptorEm);
//        res=FCSmultiTauStepwise(fd->tau,fd->ACF_Dem_Dex,fd->ACF_Aem_Dex,fd->CCF_Dex,fd->ACF_sumAD_Dex,ad->arrivalTimeCh1, ad->arrivalTimeCh2, ad->markerTimeDexc,ad->markerTimeAexc, fd->subpopulation, ad->FCSparams);
        t.tick();

        res=FCSmultiTauStepwiseSimple(fd->tau,fd->ACF_Dem_Dex,TOA_Dem_Dex,ad->FCSparams);
        fd->ACF_Dem_Dex.set_numberOfPhotonsUsed(TOA_Dem_Dex.size());
        res+=FCSmultiTauStepwiseSimple(fd->tau,fd->ACF_Aem_Dex,TOA_Aem_Dex,ad->FCSparams);
        fd->ACF_Aem_Dex.set_numberOfPhotonsUsed(TOA_Aem_Dex.size());
        res+=FCSmultiTauStepwiseSimple(fd->tau,fd->ACF_Aem_Aex,TOA_Aem_Aex,ad->FCSparams);
        fd->ACF_Aem_Aex.set_numberOfPhotonsUsed(TOA_Aem_Aex.size());
        res+=FCSmultiTauStepwiseSimple(fd->tau,fd->ACF_sumAD_Dex,TOA_Dex,ad->FCSparams);
        fd->ACF_sumAD_Dex.set_numberOfPhotonsUsed(TOA_Dex.size());
        res+=FCSmultiTauStepwiseSimple(fd->tau,fd->ACF_Aem,TOA_Aem,ad->FCSparams);
        fd->ACF_Aem.set_numberOfPhotonsUsed(TOA_Aem.size());
        t.tockPrint("5x correlateMultiTau");

        if(fd->ACF_markerD.isEmpty()&& (ad->FCSparams.doDivideByMarkerCorr||ad->FCSparams.doDivideByLaterCorr)) {
            res+=oscillationACF(fd->ACF_marker,fd->ACF_markerD,fd->ACF_markerA,ad->FCSparams);
            fd->ACF_marker.set_FCSparams(ad->FCSparams);
            fd->ACF_marker.set_name("ACF marker");
            fd->ACF_markerD.set_FCSparams(ad->FCSparams);
            fd->ACF_markerD.set_name("ACF marker Dex");
            fd->ACF_markerA.set_FCSparams(ad->FCSparams);
            fd->ACF_markerA.set_name("ACF marker Aex");
        }

    } else if (ad->FCSparams.FCSMethod==FCSparams_t::FCS_TTTR) {

        t.tick();
        res=FCS_TTTR(fd->tau,fd->ACF_Dem_Dex,fd->ACF_Aem_Dex,fd->CCF_Dex,TOA_Dem_Dex, TOA_Aem_Dex, ad->FCSparams);
        t.tockPrint("FCS_TTTR");

        t.tick();

        fd->ACF_Aem.resize(fd->tau.size());
        res+=correlateTTTR(TOA_Aem.constData(),TOA_Aem.size(),fd->ACF_Aem.data(),fd->tau.constData(),fd->tau.size());
        fd->ACF_Aem.set_numberOfPhotonsUsed(TOA_Aem.size());

//        fd->ACF_Dem.resize(fd->tau.size());
//        res+=correlateTTTR(TOA_Dem.constData(),TOA_Dem.size(),fd->ACF_Dem.data(),fd->tau.constData(),fd->tau.size());

        fd->ACF_sumAD_Dex.resize(fd->tau.size());
        res+=correlateTTTR(TOA_Dex.constData(),TOA_Dex.size(),fd->ACF_sumAD_Dex.data(),fd->tau.constData(),fd->tau.size());

        fd->ACF_Aem_Aex.resize(fd->tau.size());
        res+=correlateTTTR(TOA_Aem_Aex.constData(),TOA_Aem_Aex.size(),fd->ACF_Aem_Aex.data(),fd->tau.constData(),fd->tau.size());

        QVector<double> tauOsc;
        for(int i=0;i<fd->tau.size();++i) tauOsc.append(fd->tau.at(i)+ad->FCSparams.CorrelationWindow);
        fd->ACF_Dem_Dex_later.resize(tauOsc.size());
        res+=correlateTTTR(TOA_Dem_Dex.constData(),TOA_Dem_Dex.size(),fd->ACF_Dem_Dex_later.data(),tauOsc.constData(),tauOsc.size());



        t.tockPrint("5x correlateTTTR");
        t.tick();

        if(fd->ACF_markerD.isEmpty()&& (ad->FCSparams.doDivideByMarkerCorr||ad->FCSparams.doDivideByLaterCorr)) {
            oscillationACF_TTTR(fd->ACF_marker,fd->ACF_markerD,fd->ACF_markerA,fd->tau,ad->FCSparams);
            fd->ACF_marker.set_FCSparams(ad->FCSparams);
            fd->ACF_marker.set_name("ACF marker");
            fd->ACF_markerD.set_FCSparams(ad->FCSparams);
            fd->ACF_markerA.set_FCSparams(ad->FCSparams);
        }
//             fd->ACF_markerD.resize(fd->tau.size());
//             int n=qMin(ad->markerTimeDexc.size(),(int)1e6);
//             for(int i=0;i<n;++i) markerD.append(ad->markerTimeDexc.at(i));
//             res+=correlateTTTR(markerD.constData(),markerD.size(),fd->ACF_markerD.data(),fd->tau.constData(),fd->tau.size());

//             fd->ACF_markerA.resize(fd->tau.size());
//             n=qMin(ad->markerTimeAexc.size(),(int)1e6);
//             for(int i=0;i<n;++i) markerA.append(ad->markerTimeAexc.at(i));
//             res+=correlateTTTR(markerA.constData(),markerA.size(),fd->ACF_markerA.data(),fd->tau.constData(),fd->tau.size());
//         }

//         if(fd->ACF_marker.isEmpty()) {
//             mergeSorted(markerDA,markerD,markerA);
//             fd->ACF_marker.resize(fd->tau.size());
//             res+=correlateTTTR(markerDA.constData(),markerDA.size(),fd->ACF_marker.data(),fd->tau.constData(),fd->tau.size());
//         }

        t.tockPrint("3x correlate marker");

        } else {
        AlexEvalLog::warning("unknown FCSMethod");
        return res;
    }

    qDebug()<<"photons in correlation function\n"
           <<TOA_Dem_Dex.size()<<"\t"<<"ACF_Dem_Dex"<<"\n"
           <<TOA_Aem_Dex.size()<<"\t"<<"ACF_Aem_Dex"<<"\n"
           <<TOA_Aem.size()<<"\t"<<"ACF_Aem"<<"\n"
           <<TOA_Dem.size()<<"\t"<<"ACF_Dem"<<"\n"
           <<TOA_Dex.size()<<"\t"<<"ACF_sumAD_Dex"<<"\n"
           <<TOA_Aem_Aex.size()<<"\t"<<"ACF_Aem_Aex"<<"\n"
           <<TOA_Dem.size()<<"\t"<<"ACF_Dem_Dex_later"<<"\n";

    TOA_Aem_Aex.clear();
    TOA_Aem.clear();
    TOA_Dem.clear();
    TOA_Dex.clear();

    res+=analysisFCS(fd,ad);

    return res;
}


int analysisFCSburstwise(FCSdata *fd, const AlexData *ad)
{
   int res=EXIT_FAILURE;
   fd->clear();

   PublicTickTock t;
   AlexEvalLog::text("analysisFCSburstwise");

   if(ad->FCSparams.FCSMethod==FCSparams_t::FCScomplete) {
   AlexEvalLog::warning("not implemented");
   } else if(ad->FCSparams.FCSMethod==FCSparams_t::FCSstep) {
   AlexEvalLog::warning("not implemented");
   } else if (ad->FCSparams.FCSMethod==FCSparams_t::FCS_TTTR) {
   t.tick();
   if(selectSubpopulation(fd->subpopulation,ad->burstVectorDual,ad->FRETparams)==EXIT_FAILURE) return EXIT_FAILURE;
   t.tockPrint("selectSubpopulation");

   t.tick();
   int photonIdx=0;
   int startIdx, endIdx;
   QVector<double> TOA_Dex,TOA_Aem,TOA_Dem;
   QVector<double> TOA_Aem_Aex,TOA_Dem_Dex,TOA_Aem_Dex;
   QVector<double> ACF_Aem,ACF_Dem,ACF_sumAD_Dex,ACF_Aem_Aex,ACF_Aem_Dex,ACF_Dem_Dex,CCF_Dex;

   // lag times
   int S=ad->FCSparams.S; // blocks
   int m=ad->FCSparams.m; // multiply delta tau
   int P=ad->FCSparams.P; // tau values per block
   int ntau=P+(S-1)*(P-P/m);
   fd->tau.resize(ntau);
   MultiTauCorrelatorCreateTau(fd->tau.data(),S,m,P); // create log-spaced tau values in array tau. note that the values refer to the index in the array to correlate, i.e. in this case to the number of bins.
   for(int i=0;i<ntau;++i) fd->tau[i]*=ad->FCSparams.MinLagTime;
   fd->ACF_Aem.resize(ntau);
   fd->ACF_Aem_Dex.resize(ntau);
   fd->ACF_Dem.resize(ntau);
   fd->ACF_Dem_Dex.resize(ntau);
   fd->ACF_sumAD_Dex.resize(ntau);
   fd->ACF_Aem_Aex.resize(ntau);
   fd->CCF_Dex.resize(ntau);
//    for(int i=0;i<5;++i) qDebug()<<fd->ACF_Aem_Dex.at(i);
   // temporary correlation curves
   ACF_Aem.resize(ntau);
   ACF_Aem_Dex.resize(ntau);
   ACF_Dem.resize(ntau);
   ACF_Dem_Dex.resize(ntau);
   ACF_sumAD_Dex.resize(ntau);
   ACF_Aem_Aex.resize(ntau);
   CCF_Dex.resize(ntau);

   res=0;

   QList<QVector<double> > burstwiseACF_DemDex;
   burstwiseACF_DemDex.append(fd->tau);
   QList<QVector<double> > burstwiseCCF_Dex;
   burstwiseCCF_Dex.append(fd->tau);
   QList<QVector<double> > burstwiseACF_AemAex;
   burstwiseACF_AemAex.append(fd->tau);
   QList<QVector<double> > burstwiseACF_sumAD_Dex;
   burstwiseACF_sumAD_Dex.append(fd->tau);

   for(QVector<BurstDual>::const_iterator burst=fd->subpopulation.begin(); burst!=fd->subpopulation.end();++burst) {
       startIdx=burst->startIdx;
       endIdx=burst->endIdx();
       double tOffset= (ad->FCSparams.CorrelationWindow-burst->duration)/2;
       while((startIdx>0 ) && (ad->photons.at(startIdx).time>(burst->startTime-tOffset))) --startIdx;
       while((endIdx<ad->photons.size() ) && (ad->photons.at(endIdx).time<(burst->endTime()+tOffset))) ++endIdx;

#ifdef PHOTONMACRO
            for(photonIdx=startIdx;photonIdx<endIdx;++photonIdx) { // loop over photons
               // Dem_Dex
               if(isDem_Dex(ad->photons.at(photonIdx))) {
                   TOA_Dex.append(ad->photons.at(photonIdx).time);
                   TOA_Dem.append(ad->photons.at(photonIdx).time);
                   TOA_Dem_Dex.append(ad->photons.at(photonIdx).time);
               // Aem_Aex
               } else if(isAem_Aex(ad->photons.at(photonIdx))) {
                   TOA_Aem.append(ad->photons.at(photonIdx).time);
                   TOA_Aem_Aex.append(ad->photons.at(photonIdx).time);
               // Aem_Dex
               } else if(isAem_Dex(ad->photons.at(photonIdx))) {
                   TOA_Aem_Dex.append(ad->photons.at(photonIdx).time);
                   TOA_Aem.append(ad->photons.at(photonIdx).time);
                   TOA_Dex.append(ad->photons.at(photonIdx).time);
               // Dem_Aex
               } else if(isDem_Aex(ad->photons.at(photonIdx))) {
                   TOA_Dem.append(ad->photons.at(photonIdx).time);
               }
           }
#else
            for(photonIdx=startIdx;photonIdx<endIdx;++photonIdx) { // loop over photons
               // Dem_Dex
               if(ad->photons.at(photonIdx).isDem_Dex()) {
                   TOA_Dex.append(ad->photons.at(photonIdx).time);
                   TOA_Dem.append(ad->photons.at(photonIdx).time);
                   TOA_Dem_Dex.append(ad->photons.at(photonIdx).time);
               // Aem_Aex
               } else if(ad->photons.at(photonIdx).isAem_Aex()) {
                   TOA_Aem.append(ad->photons.at(photonIdx).time);
                   TOA_Aem_Aex.append(ad->photons.at(photonIdx).time);
               // Aem_Dex
               } else if(ad->photons.at(photonIdx).isAem_Dex()) {
                   TOA_Aem_Dex.append(ad->photons.at(photonIdx).time);
                   TOA_Aem.append(ad->photons.at(photonIdx).time);
                   TOA_Dex.append(ad->photons.at(photonIdx).time);
               // Dem_Aex
               } else if(ad->photons.at(photonIdx).isDem_Aex()) {
                   TOA_Dem.append(ad->photons.at(photonIdx).time);
               }
           }
#endif


        res+=correlateTTTR(TOA_Dem_Dex.constData(),TOA_Dem_Dex.size(),ACF_Dem_Dex.data(),fd->tau.constData(),ntau);
        for(int i=0;i<ntau;++i) fd->ACF_Dem_Dex[i] += ACF_Dem_Dex.at(i);
        burstwiseACF_DemDex.append(ACF_Dem_Dex);

//        res+=correlateTTTR(TOA_Aem_Dex.constData(),TOA_Aem_Dex.size(),ACF_Aem_Dex.data(),fd->tau.constData(),ntau);
//        for(int i=0;i<ntau;++i) fd->ACF_Aem_Dex[i] += ACF_Aem_Dex.at(i);

        res+=crosscorrelateTTTR(TOA_Dem_Dex.constData(),TOA_Dem_Dex.size(),TOA_Aem_Dex.constData(),TOA_Aem_Dex.size(),CCF_Dex.data(),fd->tau.constData(),ntau);
        for(int i=0;i<ntau;++i) fd->CCF_Dex[i] += CCF_Dex.at(i);
        burstwiseCCF_Dex.append(CCF_Dex);

//        res+=correlateTTTR(TOA_Aem.constData(),TOA_Aem.size(),ACF_Aem.data(),fd->tau.constData(),ntau);
//        for(int i=0;i<ntau;++i) fd->ACF_Aem[i] += ACF_Aem.at(i);

//        res+=correlateTTTR(TOA_Dem.constData(),TOA_Dem.size(),ACF_Dem.data(),fd->tau.constData(),ntau);
//        for(int i=0;i<ntau;++i) fd->ACF_Dem[i] += ACF_Dem.at(i);

        res+=correlateTTTR(TOA_Dex.constData(),TOA_Dex.size(),ACF_sumAD_Dex.data(),fd->tau.constData(),ntau);
        for(int i=0;i<ntau;++i) fd->ACF_sumAD_Dex[i] += ACF_sumAD_Dex.at(i);
        burstwiseACF_sumAD_Dex.append(ACF_sumAD_Dex);

        res+=correlateTTTR(TOA_Aem_Aex.constData(),TOA_Aem_Aex.size(),ACF_Aem_Aex.data(),fd->tau.constData(),ntau);
        for(int i=0;i<ntau;++i) fd->ACF_Aem_Aex[i] += ACF_Aem_Aex.at(i);
        burstwiseACF_AemAex.append(ACF_Aem_Aex);

        // set photon vectors to zero. correlation functions will be set to zero in correlateTTTR()
        TOA_Dex.clear();
        TOA_Aem.clear();
        TOA_Dem.clear();
        TOA_Aem_Aex.clear();
        TOA_Dem_Dex.clear();
        TOA_Aem_Dex.clear();
   }
   int n=fd->subpopulation.size();
   for(int i=0;i<ntau;++i) fd->ACF_Dem_Dex[i] /= n;
   for(int i=0;i<ntau;++i) fd->ACF_Aem_Dex[i] /= n;
   for(int i=0;i<ntau;++i) fd->ACF_Aem[i] /= n;
   for(int i=0;i<ntau;++i) fd->ACF_Dem[i] /= n;
   for(int i=0;i<ntau;++i) fd->ACF_sumAD_Dex[i] /= n;
   for(int i=0;i<ntau;++i) fd->ACF_Aem_Aex[i] /= n;
   for(int i=0;i<ntau;++i) fd->CCF_Dex[i] /= n;


   t.tockPrint((QString::number(fd->subpopulation.size()) + " correlated bursts").toStdString().c_str());
   t.tick();

   if(fd->ACF_markerD.isEmpty()&& (ad->FCSparams.doDivideByMarkerCorr||ad->FCSparams.doDivideByLaterCorr)) {
       oscillationACF_TTTR(fd->ACF_marker,fd->ACF_markerD,fd->ACF_markerA,fd->tau,ad->FCSparams);
   }

   t.tockPrint("3x correlate marker");

    t.tick();
    double normalization=0;
    for(int i=1;i<burstwiseACF_DemDex.size();++i) { // i=1 to skip tau
        int j=0;
        while(j<fd->ACF_markerD.size()) {
            normalization=fd->ACF_markerD.at(j);
            burstwiseACF_DemDex[i][j]/=normalization;
           ++j;
        }
    }
    for(int i=1;i<burstwiseACF_AemAex.size();++i) { // i=1 to skip tau
        int j=0;
        while(j<fd->ACF_markerA.size()) {
            normalization=fd->ACF_markerA.at(j);
            burstwiseACF_AemAex[i][j]/=normalization;
            ++j;
        }
    }
    for(int i=1;i<burstwiseCCF_Dex.size();++i) { // i=1 to skip tau
        int j=0;
        while(j<fd->ACF_markerD.size()) {
            normalization=fd->ACF_markerD.at(j);
            burstwiseCCF_Dex[i][j]/=normalization;
            ++j;
        }
    }
    for(int i=1;i<burstwiseACF_sumAD_Dex.size();++i) { // i=1 to skip tau
        int j=0;
        while(j<fd->ACF_markerD.size()) {
            normalization=fd->ACF_markerD.at(j);
            burstwiseACF_sumAD_Dex[i][j]/=normalization;
            ++j;
        }
    }

   QString filename=ad->currentDir+"/"+ad->name+"_burstwiseFCS_DemDex.csv";
   qDebug()<<"writing "<<burstwiseACF_DemDex.size()<<"correlation functions to file"<<filename;
   writeListtoCSV(burstwiseACF_DemDex,ad->FCSparams.toString(),filename,",");
   filename=ad->currentDir+"/"+ad->name+"_burstwiseACF_AemAex.csv";
   qDebug()<<"writing "<<burstwiseACF_AemAex.size()<<"correlation functions to file"<<filename;
   writeListtoCSV(burstwiseACF_AemAex,ad->FCSparams.toString(),filename,",");
   filename=ad->currentDir+"/"+ad->name+"_burstwiseCCF_Dex.csv";
   qDebug()<<"writing "<<burstwiseCCF_Dex.size()<<"correlation functions to file"<<filename;
   writeListtoCSV(burstwiseCCF_Dex,ad->FCSparams.toString(),filename,",");
   filename=ad->currentDir+"/"+ad->name+"_burstwiseACF_sumAD_Dex.csv";
   qDebug()<<"writing "<<burstwiseACF_sumAD_Dex.size()<<"correlation functions to file"<<filename;
   writeListtoCSV(burstwiseACF_sumAD_Dex,ad->FCSparams.toString(),filename,",");

   t.tockPrint("divided by marker burstwiseACF_DemDex and written to file");

   } else {
   AlexEvalLog::warning("unknown FCSMethod");
   return EXIT_FAILURE;
   }

   // filter etc.
   res=analysisFCS(fd, ad);

   return res;
}

// 23.09.13
int analysisFCS_OLD(FCSdata *fd, AlexData *ad)
{
   int res=EXIT_FAILURE;
   fd->clear();

   if(ad->FCSparams.FCSMethod==FCSparams_t::FCScomplete) {
       res=FCSmultiTauBinned(ad->arrivalTimeCh1,ad->arrivalTimeCh2, ad->FCSparams);
   } else if(ad->FCSparams.FCSMethod==FCSparams_t::FCSstep) {
       if(selectSubpopulation(fd->subpopulation,ad->burstVectorDual,ad->FRETparams)==EXIT_FAILURE) return EXIT_FAILURE;
       res=FCSmultiTauStepwise(fd->tau,fd->ACF_Dem_Dex,fd->ACF_Aem_Dex,fd->CCF_Dex,fd->ACF_sumAD_Dex,ad->arrivalTimeCh1, ad->arrivalTimeCh2, ad->markerTimeDexc,ad->markerTimeAexc, fd->subpopulation, ad->FCSparams);
   } else if (ad->FCSparams.FCSMethod==FCSparams_t::FCS_TTTR) {
       if(selectSubpopulation(fd->subpopulation,ad->burstVectorDual,ad->FRETparams)==EXIT_FAILURE) return EXIT_FAILURE;
//        res=FCS_TTTR_OLD(fd->tau,fd->ACF_Dem_Dex,fd->ACF_Aem_Dex,fd->CCF_Dex,ad->arrivalTimeCh1, ad->arrivalTimeCh2, ad->markerTimeDexc,ad->markerTimeAexc, fd->subpopulation, ad->FCSparams);

       int markerIdx=0,lastD=0,lastA=0;
       markerIdx=0;lastD=0;lastA=0;
       QVector<double> TOA_Dex;

       for(QVector<BurstDual>::const_iterator burst=fd->subpopulation.begin(); burst!=fd->subpopulation.end();++burst) {
           lastD=burst->startIdxDonor;
           lastA=burst->startIdxAcceptor;
           while(ad->markerTimeDexc.at(markerIdx)<burst->startTime) ++markerIdx;
           --markerIdx;
           while(ad->markerTimeDexc.at(markerIdx)<burst->endTime()) {
               while(ad->arrivalTimeCh1.at(lastD)<ad->markerTimeDexc.at(markerIdx)) ++lastD; // advance to begin of Dexc
               while(ad->arrivalTimeCh1.at(lastD)<ad->markerTimeAexc.at(markerIdx)) { // count photons to the begin of Aex (= end of Dex)
                   TOA_Dex.append(ad->arrivalTimeCh1.at(lastD));
                   ++lastD;
               }
               while(ad->arrivalTimeCh2.at(lastA)<ad->markerTimeDexc.at(markerIdx)) ++lastA;
               while(ad->arrivalTimeCh2.at(lastA)<ad->markerTimeAexc.at(markerIdx)) {
                   TOA_Dex.append(ad->arrivalTimeCh2.at(lastA));
                   ++lastA;
               }
               ++markerIdx;
           }
       }

       fd->ACF_sumAD_Dex.resize(fd->tau.size());
       res+=correlateTTTR(TOA_Dex.constData(),TOA_Dex.size(),fd->ACF_sumAD_Dex.data(),fd->tau.constData(),fd->tau.size());
       TOA_Dex.clear();


       markerIdx=0;lastD=0;lastA=0;
       QVector<double> TOA_Aex;

       for(QVector<BurstDual>::const_iterator burst=fd->subpopulation.begin(); burst!=fd->subpopulation.end();++burst) {
           lastD=burst->startIdxDonor;
           lastA=burst->startIdxAcceptor;
           while(ad->markerTimeDexc.at(markerIdx)<burst->startTime) ++markerIdx;
           --markerIdx;
           while(((markerIdx+1)<ad->markerTimeDexc.size()) && (ad->markerTimeDexc.at(markerIdx)<burst->endTime())) {
               while(ad->arrivalTimeCh1.at(lastD)<ad->markerTimeAexc.at(markerIdx)) ++lastD; // advance to begin of Aex
               while(ad->arrivalTimeCh1.at(lastD)<ad->markerTimeDexc.at(markerIdx+1)) { // count photons to the begin of Dex (= end of Aex)
                   TOA_Aex.append(ad->arrivalTimeCh1.at(lastD));
                   ++lastD;
               }
               while(ad->arrivalTimeCh2.at(lastA)<ad->markerTimeAexc.at(markerIdx)) ++lastA;
               while(ad->arrivalTimeCh2.at(lastA)<ad->markerTimeDexc.at(markerIdx+1)) {
                   TOA_Aex.append(ad->arrivalTimeCh2.at(lastA));
                   ++lastA;
               }
               ++markerIdx;
           }
       }

       fd->ACF_sumAD_Dex.resize(fd->tau.size());
       res+=correlateTTTR(TOA_Aex.constData(),TOA_Aex.size(),fd->ACF_Aem.data(),fd->tau.constData(),fd->tau.size());


       markerIdx=0;lastD=0;lastA=0;
       QVector<double> TOA_Aem_Aex;

       for(QVector<BurstDual>::const_iterator burst=fd->subpopulation.begin(); burst!=fd->subpopulation.end();++burst) {
           lastA=burst->startIdxAcceptor;
           while(ad->markerTimeDexc.at(markerIdx)<burst->startTime) ++markerIdx;
           --markerIdx;
           while(((markerIdx+1)<ad->markerTimeDexc.size()) && (ad->markerTimeDexc.at(markerIdx)<burst->endTime())) {
               while(ad->arrivalTimeCh2.at(lastA)<ad->markerTimeAexc.at(markerIdx)) ++lastA;
               while(ad->arrivalTimeCh2.at(lastA)<ad->markerTimeDexc.at(markerIdx+1)) {
                   TOA_Aem_Aex.append(ad->arrivalTimeCh2.at(lastA));
                   ++lastA;
               }
               ++markerIdx;
           }
       }

       fd->ACF_Aem_Aex.resize(fd->tau.size());
       res+=correlateTTTR(TOA_Aem_Aex.constData(),TOA_Aem_Aex.size(),fd->ACF_Aem_Aex.data(),fd->tau.constData(),fd->tau.size());
       TOA_Aem_Aex.clear();


       res+=correlateTTTR(ad->markerTimeDexc.constData(),ad->markerTimeDexc.size(),fd->ACF_markerD.data(),fd->tau.constData(),fd->tau.size());

       } else {
       AlexEvalLog::warning("unknown FCSMethod");
       return res;
   }
   // FIXME: maybe better do this only with the markers used during the burst calculation
//    if(ad->FCSparams.doDivideByMarkerCorr) {
//        fd->oscCorr.resize(fd->tau.size());
//        correlateTTTR(ad->markerTimeDexc.constData(),ad->markerTimeDexc.size(),fd->oscCorr.data(),fd->tau.data(),fd->tau.size());
//        for(int i=0;i<fd->tau.size();++i) {
//            if(fd->oscCorr.at(i)>0) {
//                if(fd->autoCorrDonor.size()==fd->tau.size()) fd->autoCorrDonor[i]/=fd->oscCorr.at(i);
//                if(fd->autoCorrAcceptor.size()==fd->tau.size()) fd->autoCorrAcceptor[i]/=fd->oscCorr.at(i);
//                if(fd->autoCorrSum.size()==fd->tau.size()) fd->autoCorrSum[i]/=fd->oscCorr.at(i);
//                if(fd->crossCorr.size()==fd->tau.size()) fd->crossCorr[i]/=fd->oscCorr.at(i);
//            }
//        }
//    }

   if(ad->FCSparams.doDivideByMarkerCorr) {
       fd->ACF_marker.clear();
       oscillationACF(fd->ACF_marker,fd->ACF_markerD,fd->ACF_markerA,ad->FCSparams);
       for(int i=0;i<fd->tau.size();++i) {
           if(fd->ACF_markerD.at(i)>0) { // for correlation only during donor excitation. Have to divide by oscCorrAex if that changes to acceptor exc.
               if(fd->ACF_Dem_Dex.size()==fd->tau.size()) fd->ACF_Dem_Dex[i]/=fd->ACF_markerD.at(i);
               if(fd->ACF_Aem_Dex.size()==fd->tau.size()) fd->ACF_Aem_Dex[i]/=fd->ACF_markerD.at(i);
               if(fd->ACF_sumAD_Dex.size()==fd->tau.size()) fd->ACF_sumAD_Dex[i]/=fd->ACF_markerD.at(i);
               if(fd->CCF_Dex.size()==fd->tau.size()) fd->CCF_Dex[i]/=fd->ACF_markerD.at(i);
           }
       }
   }
   if(ad->FCSparams.doBoxFiltering) {
       QVector<double> corrCorrected;
       double timeWin=ad->FCSparams.BoxFilterTimeWindow;

       qDebug()<<"Box filtering procedure\ntimeWin="<<timeWin*1e6;
       boxFilter(corrCorrected,fd->tau,fd->ACF_Dem_Dex,timeWin);
       corrCorrected.swap(fd->ACF_Dem_Dex);
       corrCorrected.clear();
       boxFilter(corrCorrected,fd->tau,fd->ACF_Aem_Dex,timeWin);
       corrCorrected.swap(fd->ACF_Aem_Dex);
       corrCorrected.clear();
       boxFilter(corrCorrected,fd->tau,fd->ACF_sumAD_Dex,timeWin);
       corrCorrected.swap(fd->ACF_sumAD_Dex);
       corrCorrected.clear();
       boxFilter(corrCorrected,fd->tau,fd->CCF_Dex,timeWin);
       corrCorrected.swap(fd->CCF_Dex);
       corrCorrected.clear();
//        double idxWin=ad->FCSparams.indexWin;
//        boxFilter(corrCorrected,fd->corrAcceptor,idxWin);
//        corrCorrected.swap(corrAcceptor);
//        corrCorrected.clear();

   }
   return res;
}


int burstPhotonsToChannel(QVector<double> &tDexDem,QVector<double> &tDexAem,QVector<double> &tAexAem,
                          const Photons &photons, const QVector<BurstDual> &dualBurstVector, const FCSparams_t FCSparams  )
{

    qDebug()<<"burstPhotonsToChannel for "<<dualBurstVector.size()<<" bursts from "<<photons.size()<<" photons";
    if((tDexDem.size()>0)||(tDexAem.size()>0)||(tAexAem.size()>0)) {
        AlexEvalLog::warning("warning: overwriting data");
        tDexDem.clear();
        tDexAem.clear();
        tDexAem.clear();
    }

    int photonIdx=0;
    int countDemAex=0;

    int startIdx, endIdx,lastIdx=0;
    for(QVector<BurstDual>::const_iterator burst=dualBurstVector.begin(); burst!=dualBurstVector.end();++burst) {
        startIdx=burst->startIdx;
        endIdx=burst->endIdx();
        double tOffset= (FCSparams.CorrelationWindow-burst->duration)/2;
        double tStart=burst->startTime-tOffset;
        double tStop =burst->endTime()+tOffset;
        while((startIdx>lastIdx ) && (photons.at(startIdx).time>(tStart))) --startIdx;
        if(lastIdx>=startIdx) ++startIdx;
        while((endIdx<photons.size() ) && (photons.at(endIdx).time<(tStop))) ++endIdx;
        lastIdx=endIdx;

#ifdef PHOTONMACRO
        for(photonIdx=startIdx;photonIdx<endIdx;++photonIdx) { // loop over photons
           // Dem_Dex
           if(isDem_Dex(photons.at(photonIdx))) {
               tDexDem.append(photons.at(photonIdx).time);
           // Aem_Aex
           } else if(isAem_Aex(photons.at(photonIdx))) {
               tAexAem.append(photons.at(photonIdx).time);
           // Aem_Dex
           } else if(isAem_Dex(photons.at(photonIdx))) {
               tDexAem.append(photons.at(photonIdx).time);
           // Dem_Aex
           } else if(isDem_Aex(photons.at(photonIdx))) {
               ++countDemAex;
           }
       }
#else
        for(photonIdx=startIdx;photonIdx<endIdx;++photonIdx) { // loop over photons
           // Dem_Dex
           if(photons.at(photonIdx).isDem_Dex()) {
               tDexDem.append(photons.at(photonIdx).time);
           // Aem_Aex
           } else if(photons.at(photonIdx).isAem_Aex()) {
               tAexAem.append(ad->photons.at(photonIdx).time);
           // Aem_Dex
           } else if(photons.at(photonIdx).isAem_Dex()) {
               tDexAem.append(ad->photons.at(photonIdx).time);
           // Dem_Aex
           } else if(photons.at(photonIdx).isDem_Aex()) {
                ++countDemAex;
           }
       }
#endif
    }

    qDebug()<<"photons in channels: DexDem: "<<tDexDem.size()<<" DexAem: "<<tDexAem.size()<<" AexAem: "<<tAexAem.size();

    return EXIT_SUCCESS;
}


int analysisFCS(FCSdata *fd, const AlexData *ad)
{
    qDebug("analysisFCS");
    // FIXME: maybe better do this only with the markers used during the burst calculation
   double normalization=0;
   if(ad->FCSparams.doDivideByMarkerCorr) {
        qDebug("doDivideByMarkerCorr");
        int i =0;
        while(i<fd->ACF_markerD.size()) {
            if(fd->ACF_markerD.at(i)>0) { // for correlation only during donor excitation. Have to divide by oscCorrAex if that changes to acceptor exc.
                normalization=fd->ACF_markerD.at(i);
            } else if (ad->FCSparams.doIgnoreZeros) {
                fd->remove(i);
                continue;
            } else {
               normalization=1;
            }
            if(fd->ACF_Dem_Dex.size()==fd->tau.size()) fd->ACF_Dem_Dex[i]/=normalization;
            if(fd->ACF_Aem_Dex.size()==fd->tau.size()) fd->ACF_Aem_Dex[i]/=normalization;
            if(fd->ACF_sumAD_Dex.size()==fd->tau.size()) fd->ACF_sumAD_Dex[i]/=normalization;
            if(fd->CCF_Dex.size()==fd->tau.size()) fd->CCF_Dex[i]/=normalization;
            ++i;
        }
        i=0;
        while(i<fd->ACF_markerA.size()) {
            if(fd->ACF_markerA.at(i)>0) { // for correlation only during donor excitation. Have to divide by oscCorrAex if that changes to acceptor exc.
                normalization=fd->ACF_markerA.at(i);
            } else {
               normalization=1;
            }
            if(fd->ACF_Aem_Aex.size()==fd->tau.size()) fd->ACF_Aem_Aex[i]/=normalization;
            ++i;
        }
    }
   if(ad->FCSparams.doDivideByLaterCorr) {
       qDebug("doDivideByLaterCorr");
       for(int i=0;i<fd->tau.size();++i) {
           if(fd->ACF_Dem_Dex_later.at(i)>0) {
               normalization=fd->ACF_markerD.at(i);
//             } else if (i+1<fd->ACF_markerD.size()&&fd->ACF_markerD.at(i+1)>0) {
//                 normalization=fd->ACF_markerD.at(i+1);
           } else if (ad->FCSparams.doIgnoreZeros) {
               fd->remove(i);
               continue;
           } else {
              normalization=1;
           }
           if(fd->ACF_Dem_Dex.size()==fd->tau.size()) fd->ACF_Dem_Dex[i]/=normalization;
           if(fd->ACF_Aem_Dex.size()==fd->tau.size()) fd->ACF_Aem_Dex[i]/=normalization;
           if(fd->ACF_sumAD_Dex.size()==fd->tau.size()) fd->ACF_sumAD_Dex[i]/=normalization;
           if(fd->CCF_Dex.size()==fd->tau.size()) fd->CCF_Dex[i]/=normalization;
           ++i;
       }
    }
   if(ad->FCSparams.doBoxFiltering) {
        qDebug("doBoxFiltering2");
        QVector<double> corrCorrected;
        double timeWin=ad->FCSparams.BoxFilterTimeWindow;

        qDebug()<<"Box filtering procedure\ntimeWin="<<timeWin*1e6;
        boxFilter(corrCorrected,fd->tau,fd->ACF_Dem_Dex,timeWin);
        corrCorrected.swap(fd->ACF_Dem_Dex);
        corrCorrected.clear();
        boxFilter(corrCorrected,fd->tau,fd->ACF_Aem_Dex,timeWin);
        corrCorrected.swap(fd->ACF_Aem_Dex);
        corrCorrected.clear();
        boxFilter(corrCorrected,fd->tau,fd->ACF_sumAD_Dex,timeWin);
        corrCorrected.swap(fd->ACF_sumAD_Dex);
        corrCorrected.clear();
        boxFilter(corrCorrected,fd->tau,fd->CCF_Dex,timeWin);
        corrCorrected.swap(fd->CCF_Dex);
        corrCorrected.clear();
    }
   return EXIT_SUCCESS;
}

