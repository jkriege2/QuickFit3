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

#include "alexdata.h"
#include <QFileInfo>
#include "BurstSearch.h"
#include <QDebug>


void AlexData::clear()
{
    qDebug("AlexEval::clear");
    arrivalTimeCh1.clear();
    arrivalTimeCh2.clear();
    burstVectorCh1.clear();
    burstVectorCh2.clear();
    burstVectorDual.clear();
    markerTimeDexc.clear();
    markerTimeAexc.clear();
    photons.clear();

    TTTRfilename.clear();
    name.clear();
    start=0;
    stop=0;
}

int AlexData::init()
{
    int res=readFromIni(burstSearchParamsGlobal,burstSearchParamsCh1, burstSearchParamsCh2,FCSparams,FRETparams,iniFilename.toStdString());
    res += readFromIniFCS(FCSparams,iniFilename.toStdString());
    res += rangeALEX.readFromIni(iniFilename.toStdString());
    return res;
}

int AlexData::initFCS()
{
    return readFromIniFCS(FCSparams,iniFilename.toStdString());
}

bool AlexData::isEmpty() const { return photons.isEmpty(); }

bool AlexData::isEmptyMarker() const { return markerTimeDexc.isEmpty()||markerTimeAexc.isEmpty(); }

bool AlexData::isEmptyBurst() const {return burstVectorDual.isEmpty();} // {return burstVectorDonor.isEmpty()||burstVectorAcceptor.isEmpty();}

bool AlexData::isEmptyFRET() const { if(!isEmptyBurst()) return (burstVectorDual.first().proximityRatio==0&&burstVectorDual.first().stoichiometryRatio==0);
                                   else return true;} // proximityRatio is initialized to 0

int AlexData::setRange()
{
    int result=0;
    QString text("");
    int idxStart=0,idxStop=0,n;
    if(!markerTimeDexc.isEmpty()) {
        if (start<markerTimeDexc.first()&&stop>markerTimeAexc.last()) {
            qWarning("out of range");
            return -1;
        }
    }

    if (start>stop) {
        qWarning("start>stop");
        result=-1;
    } else {
        text+=text.sprintf("setting range. start=%f\tstop=%f",start,stop);
        if(markerTimeDexc.isEmpty()) {
            start=photons.first().time;
            stop=photons.last().time;
        } else {
//            while(idxStart<markerTimeDexc.size() && markerTimeDexc.at(idxStart)<start) ++idxStart;
//            markerTimeDexc.remove(0,idxStart);
//            n=markerTimeDexc.size();
//            while(idxStop<n && markerTimeDexc.at(idxStop)<=stop) ++idxStop;
//            markerTimeDexc.remove(idxStop,markerTimeDexc.size()-idxStop);
//            text+=text.sprintf("removed %i entries in front and %i at the end of markerTimeDexc",idxStart,n-idxStop);

//            idxStart=0;idxStop=0;
//            while(idxStart<markerTimeAexc.size() && markerTimeAexc.at(idxStart)<start) ++idxStart;
//            markerTimeAexc.remove(0,idxStart);
//            n=markerTimeAexc.size();
//            while(idxStop<n && markerTimeAexc.at(idxStop)<=stop) ++idxStop;
//            markerTimeAexc.remove(idxStop,markerTimeAexc.size()-idxStop);
//            text+=text.sprintf("removed %i entries in front and %i at the end of markerTimeAexc",idxStart,n-idxStop);

            while (markerTimeDexc.first()>markerTimeAexc.first()) markerTimeAexc.pop_front(); // assure that donor channel has the first marker event
            while(markerTimeDexc.last()>markerTimeAexc.last()) markerTimeDexc.pop_back(); // assure that acceptor channel has the last marker event
            start=markerTimeDexc.first();
            stop=markerTimeAexc.last();
        }
        text+=text.sprintf("adjusted range: start=%f\tstop=%f",start,stop);

        idxStart=0;idxStop=0;
        while( idxStart<arrivalTimeCh1.size() && arrivalTimeCh1.at(idxStart)<start) ++idxStart;
        arrivalTimeCh1.remove(0,idxStart);
        n=arrivalTimeCh1.size();
        while(idxStop<n && arrivalTimeCh1.at(idxStop)<=stop) ++idxStop;
        arrivalTimeCh1.remove(idxStop,arrivalTimeCh1.size()-idxStop);
        text+=text.sprintf("removed %i entries in front and %i at the end of arrivalTimeDonor",idxStart,n-idxStop);

        idxStart=0;idxStop=0;
        while(idxStart<arrivalTimeCh2.size() && arrivalTimeCh2.at(idxStart)<start) ++idxStart;
        arrivalTimeCh2.remove(0,idxStart);
        n=arrivalTimeCh2.size();
        while(idxStop<n && arrivalTimeCh2.at(idxStop)<=stop) ++idxStop;
        arrivalTimeCh2.remove(idxStop,arrivalTimeCh2.size()-idxStop);
        text+=text.sprintf("removed %i entries in front and %i at the end of arrivalTimeAcceptor",idxStart,n-idxStop);

        AlexEvalLog::text(text);


//        while(markerTimeDexc.first()<start) markerTimeDexc.pop_front();
//        while(markerTimeAexc.first()<start) markerTimeAexc.pop_front();
//        while(arrivalTimeDonor.last()>stop) arrivalTimeDonor.pop_back();
//        while(arrivalTimeAcceptor.last()>stop) arrivalTimeAcceptor.pop_back();
//        while(markerTimeDexc.last()>stop) markerTimeDexc.pop_back();
//        while(markerTimeAexc.last()>stop) markerTimeAexc.pop_back();
    }
    return result;
}

void AlexData::setName(const QString &t3rFile)
{
    if(t3rFile.isEmpty()) {
        TTTRfilename="";
        name="";
    } else {
        TTTRfilename=t3rFile;
        name=QFileInfo(TTTRfilename).completeBaseName();
        currentDir=QFileInfo(TTTRfilename).absolutePath();
    }
}

void AlexData::setBSMethodSingle(const BurstSearchParams_t::BurstSearchMethodsSingle methodSingle)
{
    burstSearchParamsGlobal.burstSearchMethodSingle=methodSingle;
    burstSearchParamsCh1.burstSearchMethodSingle=methodSingle;
    burstSearchParamsCh2.burstSearchMethodSingle=methodSingle;
//    qDebug("AlexData::setBSMethodSinge set BS method to %i", index);
}

void AlexData::setBSMethodDual(const BurstSearchParams_t::BurstSearchMethodsDual methodDual)
{
    burstSearchParamsGlobal.burstSearchMethodDual=methodDual;
    burstSearchParamsCh1.burstSearchMethodDual=methodDual;
    burstSearchParamsCh2.burstSearchMethodDual=methodDual;
//    qDebug("AlexData::setBSMethodDual set BS method to %i", index);

}

void AlexData::save(const QString &filename) const
{
    QFile file;
    if(filename.isEmpty()) {AlexEvalLog::warning("AlexData::save: enter a valid filename"); return;}
    else file.setFileName(filename);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) AlexEvalLog::warning(file.errorString());
    QTextStream out(&file); out.setRealNumberPrecision(11);
    out <<"# AlexData of " << TTTRfilename << "\n";
    out << burstSearchParamsGlobal.toString("# ","\t")<<"\n";
    out << FRETparams.toString("# ","\t")<<"\n";

    if(burstVectorDual.size()>0) {
        out <<"# "<<burstVectorDual.size()<<" bursts were analyzed.\n";
        out<<burstVectorDual.first().toStringHeader().c_str()<<"\n";
        for(int i=0;i<burstVectorDual.size();++i) {
            out<<burstVectorDual.at(i).toString("","\t").c_str()<<"\n";
        }
        AlexEvalLog::text(QString::number(burstVectorDual.size())+" bursts written to file "+file.fileName());
    } else AlexEvalLog::warning("No dual bursts to save");
    file.close();
}

void AlexData::saveIni(const QString &filename) const
{
    AlexEvalLog::text("writing parameters to ini file");
    if(filename.isEmpty()) {
        writeToIni(burstSearchParamsGlobal,burstSearchParamsCh1, burstSearchParamsCh2,FCSparams,FRETparams,iniFilename.toStdString());
        rangeALEX.writeToIni(iniFilename.toStdString());
        } else if(filename==iniFilename) {
            AlexEvalLog::warning("overwriting ini file");
            writeToIni(burstSearchParamsGlobal,burstSearchParamsCh1, burstSearchParamsCh2,FCSparams,FRETparams,iniFilename.toStdString());
            rangeALEX.writeToIni(iniFilename.toStdString());
        } else  {
            AlexEvalLog::warning("old iniFilename "+this->iniFilename+"\tnew ini "+filename+"\nplease change iniFilename");
            writeToIni(burstSearchParamsGlobal,burstSearchParamsCh1, burstSearchParamsCh2,FCSparams,FRETparams,filename.toStdString());
            rangeALEX.writeToIni(filename.toStdString());
        }
}



QString AlexData::printParams() const
{
    QString out;
    out = this->printParamsBurstSearch();
    out+="\n";
    out+=FRETparams.toString();
    out+="\n";
    out+=FCSparams.toString();
    return out;
}

QString AlexData::printParamsBurstSearch() const
{
    QString out;
    out=burstSearchParamsGlobal.toString("Global ");
    out+="\n";
    out+=burstSearchParamsCh1.toString("Donor ");
    out+="\n";
    out+=burstSearchParamsCh2.toString("Acceptor ");
    return out;
}

QString AlexData::resultBS() const
{
    return  "#bursts = " + QString::number(burstVectorDual.size()) + "\n" +
            "#donor bursts = " + QString::number(burstVectorCh1.size()) + "\n" +
            "#acceptor bursts = " + QString::number(burstVectorCh2.size()) + "\n";
}

double AlexData::meanBurstrate() const
{
    double mean=0;
    for(int i=0;i<burstVectorDual.size();++i)
        mean += burstVectorDual.at(i).burstrate();
    return mean/burstVectorDual.size();
}

double AlexData::meanBurstDuration() const
{
    double mean=0;
    for(int i=0;i<burstVectorDual.size();++i)
        mean += burstVectorDual.at(i).duration;
    return mean/burstVectorDual.size();
}

double AlexData::meanNoOfPhotons() const
{
    double mean=0;
    for(int i=0;i<burstVectorDual.size();++i)
        mean += burstVectorDual.at(i).numberOfPhotons;
    return mean/burstVectorDual.size();
}

double AlexData::getExcitationPeriodDonor() const
{
    double res=0;
    int n=qMin(100,markerTimeDexc.size());
    for(int i=1;i<n;++i) {
        res+=markerTimeDexc.at(i)-markerTimeDexc.at(i-1);
    }
    res /= n-1;
    return res;
}

double AlexData::getExcitationPeriodAcceptor() const
{
    double res=0;
    int n=qMin(100,markerTimeAexc.size());
    for(int i=1;i<n;++i) {
        res+=markerTimeAexc.at(i)-markerTimeAexc.at(i-1);
    }
    res /= n-1;
    return res;
}

double AlexData::getExcitationDurationDonor() const
{
    double res=0;
    int n=0;
    while((n<(markerTimeAexc.size()))&&(n<markerTimeDexc.size())&&(n<100)) {
            res+=markerTimeAexc.at(n)-markerTimeDexc.at(n);
            ++n;
    }
    return res/n;
}

double AlexData::getExcitationDurationAcceptor() const
{
    double res=0;
    int n=0, i=1;
    while((i<(markerTimeAexc.size()-1))&&(i<markerTimeDexc.size())&&(i<100)) {
            res+=markerTimeDexc.at(i)-markerTimeAexc.at(i-1);
            ++i;
            ++n;
    }
    return res/n;
}

double AlexData::meanPR() const
{
    double mean=0;
    for(int i=0;i<burstVectorDual.size();++i)
        mean += burstVectorDual.at(i).proximityRatio;
    return mean/burstVectorDual.size();
}

double AlexData::meanSR() const
{
    double mean=0;
    for(int i=0;i<burstVectorDual.size();++i)
        mean += burstVectorDual.at(i).stoichiometryRatio;
    return mean/burstVectorDual.size();
}

int AlexData::countBursts(FRETparams_t &FRETparams)
{
    int count = 0;
    for(int i=0;i<burstVectorDual.size();++i)
        if(burstVectorDual.at(i).isMasked(FRETparams)) continue; // exclude burst
        else count++;
    return count;
}

gsl_histogram *AlexData::burstDurationDistribution(size_t nbins)
{
    gsl_histogram *hist =  gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform(hist,0,burstSearchParamsGlobal.MaxDuration*1e3);

    for(int i=0;i<burstVectorDual.size();++i)
        gsl_histogram_increment(hist, burstVectorDual.at(i).duration*1e3);

    return hist;
}

gsl_histogram *AlexData::burstSizeDistribution(size_t nbins, double maxBurstSize)
{
    if(maxBurstSize==0) maxBurstSize = burstSearchParamsGlobal.MaxBurstrate*burstSearchParamsGlobal.MaxDuration;
    gsl_histogram *hist =  gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform(hist,burstSearchParamsGlobal.MinBurstSize,maxBurstSize);

    for(int i=0;i<burstVectorDual.size();++i)
        gsl_histogram_increment(hist, burstVectorDual.at(i).numberOfPhotons);

    return hist;
}

gsl_histogram *AlexData::burstRateDistribution(size_t nbins)
{
    gsl_histogram *hist =  gsl_histogram_alloc(nbins);
    gsl_histogram_set_ranges_uniform(hist,burstSearchParamsGlobal.MinBurstrate*1e-3,burstSearchParamsGlobal.MaxBurstrate*1e-3);

    for(int i=0;i<burstVectorDual.size();++i)
        gsl_histogram_increment(hist, burstVectorDual.at(i).burstrate()*1e-3);

    return hist;
}

int AlexData::calculateBackground()
{
    if(calculateDutyCycles()==EXIT_SUCCESS) {
        FRETparams.bg_Dem_Dex=background.n_Dem_Dex/background.duration/FRETparams.dutyCycleDex;
        FRETparams.bg_Aem_Dex=background.n_Aem_Dex/background.duration/FRETparams.dutyCycleDex;
        FRETparams.bg_Dem_Aex=background.n_Dem_Aex/background.duration/FRETparams.dutyCycleAex;
        FRETparams.bg_Aem_Aex=background.n_Aem_Aex/background.duration/FRETparams.dutyCycleAex;
    } else if (method==swFRET) {
        FRETparams.bg_Dem_Dex=background.n_Dem_Dex/background.duration;
        FRETparams.bg_Aem_Dex=background.n_Aem_Dex/background.duration;
        FRETparams.bg_Dem_Aex=0;
        FRETparams.bg_Aem_Aex=0;
    } else {
        return EXIT_FAILURE;
    }
//        old method to calculate background rates:
//        qDebug()<<ad->background.n_Dem_Dex/t_Dex/ad->markerTimeDexc.size();
//        qDebug()<<ad->background.n_Aem_Dex/t_Dex/ad->markerTimeDexc.size();
//        qDebug()<<ad->background.n_Dem_Aex/t_Aex/ad->markerTimeAexc.size();
//        qDebug()<<ad->background.n_Aem_Aex/t_Aex/ad->markerTimeAexc.size();
    //        difference between the methods is neglegible
    return EXIT_SUCCESS;
}

int AlexData::calculateDutyCycles()
{
    double t_Dex=getExcitationDurationDonor();
    double t_Aex=getExcitationDurationAcceptor();
    if(t_Aex>0 && t_Dex>0) {
        FRETparams.dutyCycleDex=t_Dex/(t_Dex+t_Aex);
        FRETparams.dutyCycleAex=t_Aex/(t_Dex+t_Aex);
    } else {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}



AlexData::AlexData() : burstSearchParamsGlobal(),burstSearchParamsCh1(), burstSearchParamsCh2(),FRETparams(),FCSparams()
{
    qDebug("AlexData: default constructor");
    iniFilename="";
    TTTRfilename="";
    currentDir="";
    name="";
    start=0;
    stop=0;
    rangeALEX.setDefault();
    std::numeric_limits<double> l;
    max_measurementtime=1e-9/l.epsilon(); // approx. nano sec resolution
    method=nMethods;
}

AlexData::AlexData(const QString &iniFile)
{
    iniFilename=iniFile;
    readFromIni(burstSearchParamsGlobal,burstSearchParamsCh1,burstSearchParamsCh2,FCSparams,FRETparams,iniFilename.toStdString());
    qDebug("AlexData: construct from ini file");
//    printParams();
    std::numeric_limits<double> l;
    max_measurementtime=1e-9/l.epsilon(); // approx. nano sec resolution
}


///*

int SaveBursts(const QVector<Burst> &burstVector, const BurstSearchParams_t BurstSearchParams, const QString filename)
{
    QFile file;
    if(filename.isEmpty()) qFatal("SaveBursts: enter a valid filename");
    else file.setFileName(filename);

    if(burstVector.size()==0) {
        AlexEvalLog::warning("vector is empty");
        return EXIT_FAILURE;
    }
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        AlexEvalLog::warning(file.errorString());
        return EXIT_FAILURE;
    }
    QTextStream out(&file);
    out << BurstSearchParams.toString("# ","\t") //Burst Search with parameters\tM="<<BurstSearchParams.M<<"\tL="<<BurstSearchParams.L()<<" \tT="<<BurstSearchParams.T*1e6<<" us.\t"
       <<".\t" <<burstVector.size()<<" bursts were found.\n";
    out <<"# start photon \tstart time in s \tnumber of photons \tburst duration in ms\n";
    out.setRealNumberPrecision(11);
    for(int i=0;i<burstVector.size();++i) {
        out <<burstVector[i].startIdx<<"\t"<<burstVector[i].startTime<<"\t"
           <<burstVector[i].numberOfPhotons<<"\t"<<burstVector[i].duration*1e3<<"\n";
    }

    AlexEvalLog::text(burstVector.size() + "bursts written to file " + file.fileName());
    file.close();
    return EXIT_SUCCESS;

}



void SaveBurstsDual(const QVector<BurstDual> &burstVectorDual, const QString filename)
{
    QFile file;
    if(filename.isEmpty()) qFatal("SaveBursts: enter a valid filename");
    else file.setFileName(filename);
    if(burstVectorDual.size()==0) AlexEvalLog::warning("No dual bursts to save!");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) AlexEvalLog::warning(file.errorString());
    QTextStream out(&file);
    out.setRealNumberPrecision(11);
    out <<"# ALEX dual bursts\n# "<<burstVectorDual.size()<<" bursts were analyzed.\n";

    out<<burstVectorDual.first().toStringHeader().c_str();
    for(int i=0;i<burstVectorDual.size();++i) {
        out<<burstVectorDual.at(i).toString("",", ").c_str()<<"\n";
    }
    AlexEvalLog::text(burstVectorDual.size()+"bursts written to file "+file.fileName());
    file.close();

    //    out <<"# start time in s\tburst duration in ms\tproximity ratio\tstoichiometry ratio\tnumber of photons\tn_Dem_Dex\tn_Aem_Dex\tn_Dem_Aex\tn_Aem_Aex\tduration Dem in ms\tduration Aem in ms\tPR_RAW\tS_RAW\ttype\n";

    //    for(int i=0;i<burstVectorDual.size();++i) {
    //        out <<burstVectorDual.at(i).startTime<<"\t"<<burstVectorDual.at(i).duration*1e3<<"\t"
    //           <<burstVectorDual.at(i).proximityRatio<<"\t"<<burstVectorDual.at(i).stoichiometryRatio<<"\t"<<burstVectorDual.at(i).numberOfPhotons<<"\t"
    //           <<burstVectorDual.at(i).n_Dem_Dex<<"\t"<<burstVectorDual.at(i).n_Aem_Dex<<"\t"<<burstVectorDual.at(i).n_Dem_Aex<<"\t"<<burstVectorDual.at(i).n_Aem_Aex<<"\t"<<burstVectorDual.at(i).durationDonor*1e3<<"\t"<<burstVectorDual.at(i).durationAcceptor*1e3<<"\t"<<burstVectorDual.at(i).proximityRatioRaw<<"\t"<<burstVectorDual.at(i).stoichiometryRatioRaw<<"\t"<<(int)burstVectorDual.at(i).type<<"\n";
    //    }

}
//*/



int BurstSearch(AlexData *ad)
{
    qDebug("BurstSearch");
    int result=EXIT_FAILURE;

    ad->arrivalTimeCh1.clear();
    ad->photons.vectorChannelMarker(ad->arrivalTimeCh1,ad->burstSearchParamsCh1.channel);
    ad->arrivalTimeCh2.clear();
    ad->photons.vectorChannelMarker(ad->arrivalTimeCh2,ad->burstSearchParamsCh2.channel);

    ad->setRange();

    if(ad->isEmpty()) {
        AlexEvalLog::warning("no photon entries");
        return EXIT_FAILURE;

    } else if (ad->burstSearchParamsGlobal.burstSearchMethodDual==BurstSearchParams_t::CombineBurstsOR) {
        result =BurstSearchSingle(ad->burstVectorCh1,ad->arrivalTimeCh1,ad->burstSearchParamsCh1);
        result+=BurstSearchSingle(ad->burstVectorCh2,ad->arrivalTimeCh2,ad->burstSearchParamsCh2);
        ad->burstVectorDual.reserve((ad->burstVectorCh2.size()+ad->burstVectorCh1.size()));// reserve memory for dual bursts
        result+=CombineBurstsOR(ad->burstVectorDual,ad->burstVectorCh1,ad->burstVectorCh2,ad->photons,ad->burstSearchParamsGlobal);

    } else if (ad->burstSearchParamsGlobal.burstSearchMethodDual==BurstSearchParams_t::DualChannel){
        result =BurstSearchSingle(ad->burstVectorCh1,ad->arrivalTimeCh1,ad->burstSearchParamsCh1);
        result+=BurstSearchSingle(ad->burstVectorCh2,ad->arrivalTimeCh2,ad->burstSearchParamsCh2);
        ad->burstVectorDual.reserve((ad->burstVectorCh2.size()+ad->burstVectorCh1.size()));// reserve memory for dual bursts
        result+=DualChannelBurstSearch(ad->burstVectorDual,ad->burstVectorCh1,ad->burstVectorCh2,ad->photons,ad->burstSearchParamsGlobal);

    } else if (ad->burstSearchParamsGlobal.burstSearchMethodDual==BurstSearchParams_t::AllPhotons){
        ad->burstVectorCh2.clear();
        ad->burstVectorCh1.clear();
        result=AllPhotonsBurstSearch(ad->burstVectorDual,ad->photons,ad->burstSearchParamsGlobal,ad->burstSearchParamsGlobal);

    } else if (ad->burstSearchParamsGlobal.burstSearchMethodDual==BurstSearchParams_t::DualChannelExt){
        result =BurstSearchSingle(ad->burstVectorCh1,ad->arrivalTimeCh1,ad->burstSearchParamsCh1);
        result+=BurstSearchSingle(ad->burstVectorCh2,ad->arrivalTimeCh2,ad->burstSearchParamsCh2);
        ad->burstVectorDual.reserve((ad->burstVectorCh2.size()+ad->burstVectorCh1.size()));// reserve memory for dual bursts
        result+=DualChannelBurstSearchExt(ad->burstVectorDual,ad->burstVectorCh1,ad->burstVectorCh2,ad->photons,ad->burstSearchParamsGlobal);

    } else {
        AlexEvalLog::warning("unknown burstSearchMethodDual");
    }
    return result;
}


GMMfitData::GMMfitData()
{
    gmm=NULL;
    defaultModel();
}

void GMMfitData::defaultModel()
{
    init(LowFRET);
}

QString GMMfitData::getModelName(const GMMfitData::Model model) const
{
    QString str("");
     switch(model) {
     case LowFRET:
         str="LowFRET";
         break;
     case HighFRET:
         str="HighFRET";
         break;
     case LowHighFRET:
         str="LowHighFRET";
         break;
     case Aonly:
         str="Aonly";
         break;
     case Donly:
         str="Donly";
         break;
     case DonlyAonly:
         str="DonlyAonly";
         break;
     case LowFRETonly:
         str="LowFRETonly";
         break;
     case HighFRETonly:
         str="HighFRETonly";
         break;
     case LowHighFRETonly:
         str="LowHighFRETonly";
         break;
     }
     return str;

}

void GMMfitData::init(const Model model)
{
    delete gmm;
    GMMmodel=model;
    switch(model) {
    case LowFRET:
        gmm=new GMMfit(2,3);
        defaultDonly(gmm->getGauss(0));
        defaultAonly(gmm->getGauss(1));
        defaultLowFRET(gmm->getGauss(2));
        gmm->getPriors()[0]=0.1;
        gmm->getPriors()[1]=0.1;
        gmm->getPriors()[2]=0.8;
        break;
    case HighFRET:
        gmm=new GMMfit(2,3);
        defaultDonly(gmm->getGauss(0));
        defaultAonly(gmm->getGauss(1));
        defaultHighFRET(gmm->getGauss(2));
        gmm->getPriors()[0]=0.1;
        gmm->getPriors()[1]=0.1;
        gmm->getPriors()[2]=0.8;
        break;
    case LowHighFRET:
        gmm=new GMMfit(2,4);
        defaultDonly(gmm->getGauss(0));
        defaultAonly(gmm->getGauss(1));
        defaultLowFRET(gmm->getGauss(2));
        defaultHighFRET(gmm->getGauss(3));
        gmm->getPriors()[0]=0.1;
        gmm->getPriors()[1]=0.1;
        gmm->getPriors()[2]=0.4;
        gmm->getPriors()[3]=0.4;
        break;
    case Donly:
        gmm=new GMMfit(2,1);
        defaultDonly(gmm->getGauss(0));
        break;
    case Aonly:
        gmm=new GMMfit(2,1);
        defaultAonly(gmm->getGauss(0));
        break;
    case DonlyAonly:
        gmm=new GMMfit(2,2);
        defaultDonly(gmm->getGauss(0));
        defaultAonly(gmm->getGauss(1));
        gmm->getPriors()[0]=0.5;
        gmm->getPriors()[1]=0.5;
        break;
    case LowFRETonly:
        gmm=new GMMfit(2,1);
        defaultLowFRET(gmm->getGauss(0));
        break;
    case HighFRETonly:
        gmm=new GMMfit(2,1);
        defaultHighFRET(gmm->getGauss(0));
        break;
    case LowHighFRETonly:
        gmm=new GMMfit(2,2);
        defaultLowFRET(gmm->getGauss(0));
        defaultHighFRET(gmm->getGauss(0));
        break;
    default:
        break;

    }
    gmm->setOptions(options);
}

void GMMfitData::init()
{
    init(GMMmodel);
}

QString GMMfitData::toString() const
{
    return QString::fromStdString(gmm->toString());
}

QString GMMfitData::resultToString(const QString name) const
{
    QString str;
    QTextStream out(&str);
    for(int i=0;i<gmm->getNumberOfComponents();++i) {
        out<<name<<"\t"<<getModelName(GMMmodel)<<"\t"<<i<<"/"<<gmm->getNumberOfComponents()<<"\t"<<gmm->getPrior(i)<<"\t"<<gmm->getMu(i,0)<<"\t"
          <<gmm->getMu(i,1)<<"\t"<<gmm->getGauss(i).getSigma()(0,0)<<"\t"<<gmm->getGauss(i).getSigma()(1,1)<<"\t"
         <<gmm->getGauss(i).getSigma()(0,1)<<"\t"<<gmm->getN()<<"\n";
    }
    return str;
}

QString GMMfitData::resultToStringHeader() const
{
    return QString("name\tmodel name\tcomponent\tweight\tP\tS\tdeltaP\tdeltaS\tcorrPS\tN\n");
}


void GMMfitData::defaultAonly(Gaussian &g)
{
    Vector2d mu;
    mu<< 1.0, 0.1;
    g.setMu(mu);

    Matrix2d sigma;
    sigma<< 0.020,   -0.001,
           -0.001,    0.005;
    g.setSigma(sigma);
}

void GMMfitData::defaultDonly(Gaussian &g)
{
    Vector2d mu;
    mu<< 0.0, 0.98;
    g.setMu(mu);

    Matrix2d sigma;
    sigma<< 0.0012,    0.0000,
            0.0000,    0.0015;
    g.setSigma(sigma);
}

void GMMfitData::defaultHighFRET(Gaussian &g)
{
    Vector2d mu;
    mu<< 0.6, 0.5;
    g.setMu(mu);

    Matrix2d sigma;
    sigma<< 0.050,    0.000,
            0.000,    0.020;
    g.setSigma(sigma);
}

void GMMfitData::defaultLowFRET(Gaussian &g)
{
    Vector2d mu;
    mu<< 0.15, 0.5;
    g.setMu(mu);

    Matrix2d sigma;
    sigma<< 0.050,    0.000,
            0.000,    0.020;
    g.setSigma(sigma);
}





QString Range_t::toString(QString prefix, QString delimiter) const
{
    return  prefix +
            QString::number(minP) + " < PR < " +
            QString::number(maxP) + delimiter +
            QString::number(minS) + " < S  < " +
            QString::number(maxS) + delimiter;
}

void Range_t::check()
{
    if(minP>=maxP) {
        maxP=minP+0.1;
        AlexEvalLog::warning(toString());
    }
    if(minS>=maxS) {
        maxS=minS+0.1;
        AlexEvalLog::warning(toString());
    }
}

void Range_t::setDefault()
{
    minP=-0.1;
    maxP=1.1;
    minS=-0.1;
    maxS=1.1;
}

Range_t::Range_t()
{
    setDefault();
}

int Range_t::readFromIni(const std::string iniFilename)
{
    jkINIParser2 ini; // initialize to read the ini file
    if (iniFilename.empty()) return EXIT_FAILURE;
    Range_t defaultValuesRange;

//    AlexEvalLog::text("reading FCS parameters from " + QString::fromStdString(iniFile));
    try
    {
        ini.readFile(iniFilename); // read in an INI file

        ini.enterGroup("range");
        minP = ini.getAsDouble("minP", defaultValuesRange.minP);
        maxP = ini.getAsDouble("maxP", defaultValuesRange.maxP);
        minS = ini.getAsDouble("minS", defaultValuesRange.minS);
        maxS = ini.getAsDouble("maxS", defaultValuesRange.maxS);
        ini.leaveGroup();

    }
    catch (jkINIParser2::error e)
    { // error handling
       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
       return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Range_t::writeToIni(const std::string iniFilename) const
{
    jkINIParser2 ini; // initialize to read the ini file
    if (iniFilename.empty()) return EXIT_FAILURE;

//    AlexEvalLog::text("writing parameters to " + QString::fromStdString(iniFile));
    try
    {
        ini.readFile(iniFilename); // load previous ini data

        // append to ini data
        ini.enterGroup("range");
        ini.set("minP", minP);
        ini.set("maxP", maxP);
        ini.set("minS", minS);
        ini.set("maxS", maxS);
        ini.leaveGroup();

        ini.writeFile(iniFilename);
    }
    catch (jkINIParser2::error e)
    { // error handling
       AlexEvalLog::warning(QString::fromStdString(e.getMessage()));
       return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

