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

#include "burst.h"


//QVector<double> Photons::Dem_Dex(int start, int stop)
//{
//    QVector<double> result;
////    for(QVector<Photon>::const_iterator photon=constBegin()[start];photon!=constEnd()-stop;++photon) {
////          if(photon->isDem_Dex()) result.append(*photon);
//    for(int i=start;i<qMin(stop,size());++i) {
//        if(at(i).isDem_Dex()) result.append(at(i).time);
//    }
//    return result;
//}


void Photons::vectorChannelMarker(QVector<double>& vectorTime, const uchar _flags, const int start, int stop) const
{
    stop=qMin(stop,size());
     for(int i=start;i<stop;++i) {
#ifdef PHOTONMACRO
         if(isPhotonFlag(at(i),_flags)) vectorTime.append(at(i).time);
#else
         if(at(i).isPhotonFlag(_flags)) vectorTime.append(at(i).time);
#endif
     }
}

QVector<double> Photons::vectorChannelMarker(const uchar _flags, const int start, int stop) const
{
    QVector<double> vectorTime;
    stop=qMin(stop,size());
    for(int i=start;i<stop;++i) {
#ifdef PHOTONMACRO
         if(isPhotonFlag(at(i),_flags)) vectorTime.append(at(i).time);
#else
         if(at(i).isPhotonFlag(_flags)) vectorTime.append(at(i).time);
#endif
    }
    return vectorTime;
}

Photons::Photons():QVector<Photon>()
{
    nDem_Dex=0;
    nDem_Aex=0;
    nAem_Dex=0;
    nAem_Aex=0;
}

void Photons::countPhotons()
{
    int count_nDem_Dex=0;
    int count_nDem_Aex=0;
    int count_nAem_Dex=0;
    int count_nAem_Aex=0;

#ifdef PHOTONMACRO
    for(int i=0;i<size();++i) {
        if(isDem_Dex(at(i))) {
            ++count_nDem_Dex;
        }
        if(isDem_Aex(at(i))) {
            ++count_nDem_Aex;
        }
        if(isAem_Dex(at(i))) {
            ++count_nAem_Dex;
        }
        if(isAem_Aex(at(i))) {
            ++count_nAem_Aex;
        }
    }
#else
    for(int i=0;i<size();++i) {
        if(at(i).isDem_Dex()) {
            ++count_nDem_Dex;
        }
        if(at(i).isDem_Aex()) {
            ++count_nDem_Aex;
        }
        if(at(i).isAem_Dex()) {
            ++count_nAem_Dex;
        }
        if(at(i).isAem_Aex()) {
            ++count_nAem_Aex;
        }
    }
#endif
    nDem_Dex=count_nDem_Dex;
    nDem_Aex=count_nDem_Aex;
    nAem_Dex=count_nAem_Dex;
    nAem_Aex=count_nAem_Aex;
    
}


bool earlierThan(const Photon &p1, const Photon &p2)
{
    if(p1.time<p2.time) return true;
    else return false;
}


void Photons::sort()
{
    qSort(begin(),end(),earlierThan);
}



bool Photon::isPhotonFlags(const QVector<uint> _flags) const
{
    bool res=false;
    for(int i=0;i<_flags.size();++i){
#ifdef PHOTONMACRO
        res=res|isPhotonFlag(*this,_flags.at(i));
#else
        res=res|isPhotonFlag(_flags.at(i));
#endif
    }
    return res;
}



Burst::Burst()
{
    startTime=0;
    duration=0;
    startIdx=0;
    numberOfPhotons=0;
}


QString BurstDual::typeName() const
{
    switch(type) {
    case Unclassified:
        return QString("Unclassified");
    case SingleBurst:
        return QString("SingleBurst");
    case FRET:
        return QString("FRET");
    case NoFRET:
        return QString("NoFRET");
    case Donly:
        return QString("Donly");
    case Aonly:
        return QString("Aonly");
    case Dual:
        return QString("Dual");
    case Background:
        return QString("Background");
    }
    return QString();
}


BurstDual::BurstDual()
{
    type=Unclassified;
    startTime=0;
    duration=0;
    durationDonor=0;
    durationAcceptor=0;
    startIdx=0;
    startIdxDonor=0;
    startIdxAcceptor=0;
    numberOfPhotons=0;
    n_Dem_Dex=0;
    n_Aem_Dex=0;
    n_Dem_Aex=0;
    n_Aem_Aex=0;
    proximityRatioRaw=0;
    stoichiometryRatioRaw=0;
    proximityRatio=0;
    stoichiometryRatio=0;
    energyTransfer=0;
    stoichiometryFraction=0;
    stoichiometryRatioIntensity=0;
}

const std::string BurstDual::toString() const
{
    std::stringstream strs;

    strs<<"type="<<(int)type<<
          "\nduration="<<duration<<
          "\ndurationDonor="<<durationDonor<<
          "\ndurationAcceptor="<<durationAcceptor<<
          "\nstartDonor="<<startIdxDonor<<
          "\nstartAcceptor="<<startIdxAcceptor<<
          "\nn_Dem_Dex="<<n_Dem_Dex<<
          "\nn_Aem_Dex="<<n_Aem_Dex<<
          "\nn_Dem_Aex="<<n_Dem_Aex<<
          "\nn_Aem_Aex="<<n_Aem_Aex<<
          "\nnumberOfPhotons="<<numberOfPhotons<<
          "\nsum "<<(n_Dem_Dex + n_Aem_Dex + n_Dem_Aex + n_Aem_Aex);
    return strs.str();
}

const std::string BurstDual::toString(std::string prefix, std::string delimiter) const
{
    std::stringstream strs;
    strs<<prefix   <<startTime<<
          delimiter<<duration<<
          delimiter<<proximityRatio<<
          delimiter<<stoichiometryRatio<<
          delimiter<<numberOfPhotons<<
          delimiter<<n_Dem_Dex<<
          delimiter<<n_Aem_Dex<<
          delimiter<<n_Dem_Aex<<
          delimiter<<n_Aem_Aex<<
          delimiter<<durationDonor<<
          delimiter<<durationAcceptor<<
          delimiter<<proximityRatioRaw<<
          delimiter<<stoichiometryRatioRaw<<
          delimiter<<(int)type<<
          delimiter<<startIdxDonor<<
          delimiter<<startIdxAcceptor<<
          delimiter<<startIdx<<
          delimiter<<stoichiometryFraction<<
          delimiter<<stoichiometryRatioIntensity<<
          "";
    return strs.str();
}

const std::string BurstDual::toStringHeader(std::string prefix, std::string delimiter) const
{
    std::stringstream strs;
    strs<<prefix   << "startTime"<<
          delimiter<<"duration"<<
          delimiter<<"proximityRatio"<<
          delimiter<<"stoichiometryRatio"<<
          delimiter<<"numberOfPhotons"<<
          delimiter<<"n_Dem_Dex"<<
          delimiter<<"n_Aem_Dex"<<
          delimiter<<"n_Dem_Aex"<<
          delimiter<<"n_Aem_Aex"<<
          delimiter<<"durationDonor"<<
          delimiter<<"durationAcceptor"<<
          delimiter<<"proximityRatioRaw"<<
          delimiter<<"stoichiometryRatioRaw"<<
          delimiter<<"type"<<
          delimiter<<"startIdxDonor"<<
          delimiter<<"startIdxAcceptor"<<
          delimiter<<"startIdx"<<
          delimiter<<"stoichiometryFraction"<<
          delimiter<<"stoichiometryRatioIntensity"<<
          "";
    return strs.str();
}



