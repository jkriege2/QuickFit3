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

#include "BurstSearch.h"
#include<QDebug>
#include "ticktock.h"



#ifdef PHOTONMACRO
// count the number of photons of the burst
#define COUNTPHOTONS(photons,photonsIdx,burst) {                        \
while(photons.at(photonIdx).time<=burst.endTime()) {                    \
        ++burst.numberOfPhotons;                                        \
        if(isDem_Dex(photons.at(photonIdx))) ++burst.n_Dem_Dex;         \
        else if(isAem_Aex(photons.at(photonIdx))) ++burst.n_Aem_Aex;    \
        else if(isAem_Dex(photons.at(photonIdx))) ++burst.n_Aem_Dex;    \
        else if(isDem_Aex(photons.at(photonIdx))) ++burst.n_Dem_Aex;    \
        ++photonIdx;                                                    \
    }                                                                   \
}
#else
// count the number of photons of the new burst
#define COUNTPHOTONS(photons,photonsIdx,burst) {                        \
while(photons.at(photonIdx).time<=burst.endTime()) {                    \
        ++burst.numberOfPhotons;                                        \
        if(photons.at(photonIdx).isDem_Dex()) ++burst.n_Dem_Dex;        \
        else if(photons.at(photonIdx).isAem_Aex()) ++burst.n_Aem_Aex;   \
        else if(photons.at(photonIdx).isAem_Dex()) ++burst.n_Aem_Dex;   \
        else if(photons.at(photonIdx).isDem_Aex()) ++burst.n_Dem_Aex;   \
        ++photonIdx;                                                    \
    }                                                                   \
}
#endif

#ifdef PHOTONMACRO
#define FINDSTARTIDX(photons,photonIdx,burst) {                     \
while(photons.at(photonIdx).time<burst.startTime) ++photonIdx;      \
    burst.startIdx=photonIdx;                                       \
    int lastOther=photonIdx;                                        \
    if(isDem(photons.at(photonIdx))) {                              \
        burst.startIdxDonor=photonIdx;                              \
        while(!isAem(photons.at(lastOther))) ++lastOther;           \
        burst.startIdxAcceptor=lastOther;                           \
    } else if(isAem(photons.at(photonIdx)))  {                      \
        burst.startIdxAcceptor=photonIdx;                           \
        while(!isDem(photons.at(lastOther))) ++lastOther;           \
        burst.startIdxDonor=lastOther;                              \
    } else {                                                        \
        burst.startIdxDonor=0;                                      \
        burst.startIdxAcceptor=0;                                   \
    }                                                               \
}
#else
#define FINDSTARTIDX(photons,photonIdx,burst) {                     \
while(photons.at(photonIdx).time<burst.startTime) ++photonIdx;      \
    burst.startIdx=photonIdx;                                       \
    int lastOther=photonIdx;                                        \
    if(photons.at(photonIdx).isDem()) {                             \
        burst.startIdxDonor=photonIdx;                              \
        while(!photons.at(lastOther).isAem()) ++lastOther;          \
        burst.startIdxAcceptor=lastOther;                           \
    } else if(photons.at(photonIdx).isAem())  {                     \
        burst.startIdxAcceptor=photonIdx;                           \
        while(!photons.at(lastOther).isDem()) ++lastOther;          \
        burst.startIdxDonor=lastOther;                              \
    } else {                                                        \
        burst.startIdxDonor=0;                                      \
        burst.startIdxAcceptor=0;                                   \
    }                                                               \
}
#endif

#ifndef MYDEBUG
#define FILTERBURST(burst,BurstSearchParams) {                                                                  \
    if( burst.numberOfPhotons<BurstSearchParams.MinBurstSize) {++ignored; ++ignoredMinBurstsize; continue;}     \
    if( burst.duration>BurstSearchParams.MaxDuration) {++ignored;++ignoredMaxBurstduration; continue;}          \
    if((burst.burstrate())>BurstSearchParams.MaxBurstrate) {++ignored;++ignoredMaxBurstrate;continue;}          \
    if((burst.burstrate())<BurstSearchParams.MinBurstrate) {++ignored;++ignoredMinBurstrate;continue;}          \
    if( burst.n_Dex()<BurstSearchParams.MinBurstSizeDex) {++ignored; ++ignoredMinBurstsizeDex; continue;}       \
    if( burst.n_Aex()<BurstSearchParams.MinBurstSizeAex) {++ignored; ++ignoredMinBurstsizeAex; continue;}       \
}
#else
#define FILTERBURST(burst,BurstSearchParams) {                  \
if( burst.numberOfPhotons<BurstSearchParams.MinBurstSize) {     \
    ++ignoredMinBurstsize;++ignored;                            \
    qDebug()<<"burst ignoredMinBurstsize";                      \
    continue;}                                                  \
if( burst.duration>BurstSearchParams.MaxDuration) {             \
    ++ignoredMaxBurstduration;++ignored;                        \
    qDebug()<<"burst ignoredMaxBurstduration";                  \
    continue;}                                                  \
if((burst.burstrate())>BurstSearchParams.MaxBurstrate) {        \
    ++ignoredMaxBurstrate;++ignored;                            \
    qDebug()<<"burst ignoredMaxBurstrate";                      \
    continue;}                                                  \
if((burst.burstrate())<BurstSearchParams.MinBurstrate) {        \
    ++ignoredMinBurstrate;++ignored;                            \
    qDebug()<<"burst ignoredMinBurstrate";                      \
    continue;}                                                  \
if( burst.n_Dex()<BurstSearchParams.MinBurstSizeDex) {          \
        ++ignored; ++ignoredMinBurstsizeDex;                    \
        qDebug()<<"burst MinBurstSizeDex";                      \
        continue;}                                              \
if( burst.n_Aex()<BurstSearchParams.MinBurstSizeAex) {          \
        ++ignored; ++ignoredMinBurstsizeAex;                    \
        qDebug()<<"burst ignoredMinBurstsizeAex";               \
        continue;}                                              \
}
#endif


int DualChannelBurstSearch(QVector<BurstDual> &burstVectorDual, const QVector<Burst> &burstVectorDonor, const QVector<Burst> &burstVectorAcceptor, const Photons photons,  const BurstSearchParams_t BurstSearchParams )
{
    qDebug("DualChannelBurstSearch");
    if(burstVectorDual.size()>0) {
        AlexEvalLog::warning("Warning: overwriting data");
        burstVectorDual.clear();
    }
    if(burstVectorDonor.size()==0 || burstVectorAcceptor.size()==0) {AlexEvalLog::warning("no photon time records"); return EXIT_FAILURE;}

    // go through burst vector and find overlapping time windows.
    int last1=0,last2=0; //last indexed entries of respective burstVector
    double tend1, tend2, tend;
    double tstart1, tstart2, tstart;
    BurstDual burst;
    Burst burst1, burst2; // bursts of respective burstVector for shorter notation
    burstVectorDual.clear();
    int photonIdx=0,lastOther;
    int ignored=0; // counter for ignored bursts
    int ignoredMinBurstsize=0, ignoredMinBurstsizeDex=0, ignoredMinBurstsizeAex=0, ignoredMinBurstrate=0, ignoredMaxBurstrate=0, ignoredMaxBurstduration=0; // counter for reason of ignoring bursts

#ifdef MYDEBUG
    qDebug()<<"size of marker vector Dexc="<<markerTimeDexc.size();
    qDebug()<<"size of marker vector Aexc="<<markerTimeAexc.size();
    qDebug()<<"first burst in donor ch is from "<<burstVectorDonor.first().startTime;
    qDebug()<<"first burst in acceptor ch is from "<<burstVectorAcceptor.first().startTime;
    qDebug()<<"first marker for Donor at "<<markerTimeDexc.first();
    qDebug()<<"first marker for Acceptor at "<<markerTimeAexc.first();
    qDebug()<<"last burst in donor ch is until "<<burstVectorDonor.last().startTime+burstVectorDonor.last().duration;
    qDebug()<<"last burst in acceptor ch is until "<<burstVectorAcceptor.last().startTime+burstVectorAcceptor.last().duration;
    qDebug()<<"last marker for Donor at "<<markerTimeDexc.last();
    qDebug()<<"last marker for Acceptor at "<<markerTimeAexc.last();
#endif

    while(last1<burstVectorDonor.size()&&last2<burstVectorAcceptor.size()) { // go through all bursts

        burst1 = burstVectorDonor.at(last1);
        burst2 = burstVectorAcceptor.at(last2);

        tstart1 = burst1.startTime;
        tstart2 = burst2.startTime;
        tend1 = burst1.startTime + burst1.duration;
        tend2 = burst2.startTime + burst2.duration;

        if(tend1<tstart2) {last1++; continue;} // burst1 is before burst2, no overlap. advance one burst in channel 1.
        if(tend2<tstart1) {last2++; continue;} // burst2 is before burst1, no overlap
        // else there is a overlap
        burst.reset();

        // find burst start
        if(tstart1<tstart2) tstart = tstart2; // burst1 starts earlier. The dual burst start time is then the start time of the later burst.
        else tstart = tstart1;
        burst.startTime = tstart;

        // calculate burst duration
        if(tend1<tend2) {tend = tend1; last1++;} // burst1 ends earlier. stay at the longer burst(i.e. burst2) and look if there is another overlapping burst in channel 1
        else {tend = tend2; last2++;}
        burst.duration = tend - tstart;
        burst.durationDonor=burst1.duration;
        burst.durationAcceptor=burst2.duration;

        // count the photons in the burst
        burst.type=Dual;

        photonIdx=qMax(photonIdx,qMin(burst1.startIdx,burst2.startIdx));
        while(photons.at(photonIdx).time<burst.startTime) ++photonIdx;
        burst.startIdx=photonIdx;
        lastOther=photonIdx;
#ifdef PHOTONMACRO
        if(isDem(photons.at(lastOther))) {
            burst.startIdxDonor=photonIdx;
            while(!isAem(photons.at(lastOther))) ++lastOther;
            burst.startIdxAcceptor=lastOther;
        } else if(isAem(photons.at(lastOther)))  {
            burst.startIdxAcceptor=photonIdx;
            while(!isDem(photons.at(lastOther))) ++lastOther;
            burst.startIdxDonor=lastOther;
#else
        if(photons.at(photonIdx).isDem()) {
            burst.startIdxDonor=photonIdx;
//            tStartD=photons.at(photonIdx).time;
            while(!photons.at(lastOther).isAem()) ++lastOther;
            burst.startIdxAcceptor=lastOther;
//            tStartA=photons.at(lastOther).time;
        } else if(photons.at(photonIdx).isAem())  {
            burst.startIdxAcceptor=photonIdx;
//            tStartA=photons.at(photonIdx).time;
            while(!photons.at(lastOther).isDem()) ++lastOther;
            burst.startIdxDonor=lastOther;
//            tStartD=photons.at(lastOther).time;
#endif
        } else { // indicates error
            burst.startIdxDonor=0;
            burst.startIdxAcceptor=0;
        }

        COUNTPHOTONS(photons,photonIdx,burst);

        FILTERBURST(burst,BurstSearchParams);

#ifdef MYDEBUG
//        qDebug()<<"markeridx="<<markeridx;
        qDebug()<<burst.toString().c_str();
        if(burstVectorDual.size()>20) break;
#endif

        burstVectorDual.push_back(burst); // save burst
    }

    QString result; result=result.sprintf(
                "DCBS  found %d bursts (of %i D and %i A) and ignored %i, "
                "of which %i < MinBurstsize=%i, %i > MaxDuration=%.1fms, %i > MaxBurstrate=%.0fkHz "
                "and %i < MinBurstrate=%.0fkHz\n",
                burstVectorDual.size(),burstVectorDonor.size(),burstVectorAcceptor.size(),
                ignored,ignoredMinBurstsize,BurstSearchParams.MinBurstSize,
                ignoredMaxBurstduration, BurstSearchParams.MaxDuration*1e3,
                ignoredMaxBurstrate, BurstSearchParams.MaxBurstrate*1e-3,
                ignoredMinBurstrate, BurstSearchParams.MinBurstrate*1e-3);
    AlexEvalLog::text(result);


    return EXIT_SUCCESS;
}


int DualChannelBurstSearchExt(QVector<BurstDual> &burstVectorDual, const QVector<Burst> &burstVectorCh1, const QVector<Burst> &burstVectorCh2, const Photons &photons, const BurstSearchParams_t BurstSearchParams )
{
    qDebug("DualChannelBurstSearchExt");

    // go through burst vector and find overlapping time windows.
    int lastBurstIdx1=0,lastBurstIdx2=0; //last indexed entries of respective burstVector
    int photonIdx=0,lastOther; // last indices of photon vector
    double tEnd1, tEnd2, tEnd;
    double tStart1, tStart2, tStart;
    BurstDual burst;
    Burst burst1, burst2; // bursts of respective burstVector for shorter notation
    burstVectorDual.clear();
    bool ignoreNextCh1=false,ignoreNextCh2=false; // to ignore bursts that have been part of an overlapping burst already
    int ignored=0; // counter for ignored bursts
    int ignoredMinBurstsize=0, ignoredMinBurstsizeDex=0, ignoredMinBurstsizeAex=0, ignoredMinBurstrate=0, ignoredMaxBurstrate=0, ignoredMaxBurstduration=0; // counter for reason of ignoring bursts

    burstVectorDual.push_back(burst); // needed for the first burst, will be removed at the end
    while(lastBurstIdx2<burstVectorCh2.size()&&lastBurstIdx1<burstVectorCh1.size()) { // go through all bursts

        burst1 = burstVectorCh1.at(lastBurstIdx1);
        burst2 = burstVectorCh2.at(lastBurstIdx2);
#ifdef MYDEBUG
        qDebug("lastBurstIdx1=%i lastBurstIdx2=%i", lastBurstIdx1,lastBurstIdx2);
#endif
        tStart1 = burst1.startTime;
        tStart2 = burst2.startTime;
        tEnd1 = burst1.startTime + burst1.duration;
        tEnd2 = burst2.startTime + burst2.duration;
        burst.reset();

        if(tEnd1<tStart2) // burst1 is before burst2, no overlap. save donor burst and advance one burst in donor channel.
        {
            lastBurstIdx1++;
#ifdef MYDEBUG
            qDebug("Ch1 burst detected");
#endif
//            if(burstVectorDual.last().startTime>=burstD.startTime) continue; // then the burst was already part of an overlapping burst and must not be saved as a single burst again
            if(ignoreNextCh1) {
#ifdef MYDEBUG
                qDebug("burst was already part of an overlapping burst");
#endif
                ignoreNextCh1=false;
                continue;
            }
            burst.type=Donly;
            burst.startTime = burst1.startTime;
            tEnd = burst1.startTime + burst1.duration;
            burst.duration=burst1.duration;
            burst.durationDonor=burst1.duration;
            photonIdx=qMax(photonIdx,burst2.startIdx);
        }
        else if(tEnd2<tStart1) // burst2 is before burst1, no overlap. Save acceptor burst.
        {
            lastBurstIdx2++;
#ifdef MYDEBUG
            qDebug("Ch2 burst detected");
#endif
//            if(burstVectorDual.last().startTime>=burstA.startTime) {
            if(ignoreNextCh2) {
#ifdef MYDEBUG
                qDebug("burst was already part of an overlapping burst");
#endif
                ignoreNextCh2=false;
                continue;
            }
            burst.type=Aonly;
            burst.startTime = burst2.startTime;
            tEnd = burst2.startTime + burst2.duration;
            burst.duration=burst2.duration;
            burst.durationAcceptor=burst2.duration;
            photonIdx=qMax(photonIdx,burst2.startIdx);
        }
        else  // else there is a overlap
        {
#ifdef MYDEBUG
            qDebug("overlap detected");
#endif
            burst.type=Dual;
            // find burst start
            if(tStart1<tStart2) tStart = tStart2; // burst1 starts earlier. The dual burst start time is then the start time of the later burst.
            else tStart = tStart1;

            // calculate burst duration
            if(tEnd1<tEnd2) {tEnd = tEnd1; // burst1 ends earlier. stay at the longer burst(i.e. burstA) and look if there is another overlapping burst in channel 1
                    lastBurstIdx1++; ignoreNextCh2=true;
                }
            else {tEnd = tEnd2;
                    lastBurstIdx2++; ignoreNextCh1=true;
            }
            burst.startTime = tStart;
            burst.duration = tEnd - tStart;
            burst.durationAcceptor=burst2.duration;
            burst.durationDonor=burst1.duration;
        }

        FINDSTARTIDX(photons,photonIdx,burst)

        COUNTPHOTONS(photons,photonIdx,burst)

        FILTERBURST(burst,BurstSearchParams);

        burstVectorDual.push_back(burst); // save burst

#ifdef MYDEBUG
        qDebug()<<"item no."<<burstVectorDual.size();
        qDebug()<<"last="<<last;
        qDebug()<<burst.toString().c_str()<<"\n";
        if(burstVectorDual.size()>20) break;
#endif
    }


    while(lastBurstIdx2<burstVectorCh2.size()) {
        burst2 = burstVectorCh2.at(lastBurstIdx2);
        ++lastBurstIdx2;
#ifdef MYDEBUG
        qDebug("Ch2 burst detected");
#endif
        if(ignoreNextCh2) {
            ignoreNextCh2=false;
            continue;
        }
        burst.reset();
        burst.type=Aonly;
        burst.startTime = burst2.startTime;
        burst.duration=burst2.duration;
        burst.durationAcceptor=burst2.duration;

        FINDSTARTIDX(photons,photonIdx,burst);

        COUNTPHOTONS(photons,photonIdx,burst);

        FILTERBURST(burst,BurstSearchParams);

        burstVectorDual.push_back(burst); // save burst
    }


    while(lastBurstIdx1<burstVectorCh1.size()) {
        burst1 = burstVectorCh1.at(lastBurstIdx1);
        lastBurstIdx1++;
#ifdef MYDEBUG
        qDebug("Ch1 burst detected");
#endif
        if(ignoreNextCh1) {
            ignoreNextCh1=false;
            continue;
        }
        burst.reset();
        burst.type=Donly;
        burst.startTime = burst1.startTime;
        burst.duration=burst1.duration;
        burst.durationDonor=burst1.duration;

        FINDSTARTIDX(photons,photonIdx,burst);

        COUNTPHOTONS(photons,photonIdx,burst);

        FILTERBURST(burst,BurstSearchParams);


        burstVectorDual.push_back(burst); // save burst

    }


    ignored=ignoredMinBurstsize+ignoredMaxBurstduration+ignoredMaxBurstrate+ignoredMinBurstrate;

    burstVectorDual.pop_front(); // remove the front burst that contained no information
    QString result; result=result.sprintf(
                "AllPhotons Burst Search  found %d bursts (of %i D and %i A) and ignored %i, "
                "of which %i < MinBurstsize=%i, %i > MaxDuration=%.1fms, %i > MaxBurstrate=%.0fkHz "
                "and %i < MinBurstrate=%.0fkHz\n",
                burstVectorDual.size(),burstVectorCh1.size(),burstVectorCh2.size(),
                ignored,ignoredMinBurstsize,BurstSearchParams.MinBurstSize,
                ignoredMaxBurstduration, BurstSearchParams.MaxDuration*1e3,
                ignoredMaxBurstrate, BurstSearchParams.MaxBurstrate*1e-3,
                ignoredMinBurstrate, BurstSearchParams.MinBurstrate*1e-3);
    AlexEvalLog::text(result);

    return EXIT_SUCCESS;
}



int AllPhotonsBurstSearch(QVector<BurstDual>& burstVectorDual, const Photons& photons, const BurstSearchParams_t& BurstSearchParamsSingle,const BurstSearchParams_t& BurstSearchParamsGlobal )
{
    qDebug("AllPhotonsBurstSearch");

    if(photons.size()==0) {AlexEvalLog::warning("no photon time records"); return EXIT_FAILURE;}
    if(burstVectorDual.size()>0) {
        AlexEvalLog::warning("Warning: overwriting data");
        burstVectorDual.clear();
    }

    QVector<double> arrivalTimes; // combine all arrival times into one vector
    arrivalTimes=photons.vectorChannelMarker(BurstSearchParamsGlobal.channel);
    AlexEvalLog::text("Combine photons of channel "+QString::number(BurstSearchParamsGlobal.channel,2)+" yielding "+QString::number(arrivalTimes.size())+" photons");

    QVector<Burst> burstVector;
    BurstSearchSingle(burstVector,arrivalTimes,BurstSearchParamsSingle);

    // go through burst vector and assign the excitation periods
    int burstVecIdx=0; //last indexed entry of burstVector
    double tStartD,tStartA;
    BurstDual burst;
    Burst burstSingle; // burst of burstVector
    int photonIdx=0,lastPhotonIdx;
    int ignored=0; // counter for ignored bursts
    int ignoredMinBurstsize=0,ignoredMinBurstsizeDex=0,ignoredMinBurstsizeAex=0, ignoredMinBurstrate=0, ignoredMaxBurstrate=0, ignoredMaxBurstduration=0; // counter for reason of ignoring bursts

#ifdef MYDEBUG
    PublicTickTock t;
    double t1=0, t2=0, t3=0;
#endif

    while(burstVecIdx<burstVector.size()) { // go through all bursts
        burstSingle = burstVector.at(burstVecIdx);
        burstVecIdx++;
        burst.reset();
        burst.startTime = burstSingle.startTime;

        // count the photons in the burst
        burst.duration=burstSingle.duration;
        burst.type=SingleBurst;

//        t.tick();
#ifdef PHOTONMACRO
//        photonIdx=burstSingle.startIdx;
//        qDebug()<<photonIdx<<"\t"<<burstSingle.startIdx;
        while(photons.at(photonIdx).time<burst.startTime) ++photonIdx;
        --photonIdx;
        burst.startIdx=photonIdx;
        lastPhotonIdx=photonIdx;

//        t.tock();
//        t1=t1+t.get_duration()*1e6;
//        t.tick();

        if(isDem(photons.at(photonIdx))) {
            burst.startIdxDonor=photonIdx;
            tStartD=photons.at(photonIdx).time;
            while(!isAem(photons.at(lastPhotonIdx))) ++lastPhotonIdx;
            burst.startIdxAcceptor=lastPhotonIdx;
            tStartA=photons.at(lastPhotonIdx).time;
        } else if(isAem(photons.at(photonIdx)))  {
            burst.startIdxAcceptor=photonIdx;
            tStartA=photons.at(photonIdx).time;
            while(!isDem(photons.at(lastPhotonIdx))) ++lastPhotonIdx;
            burst.startIdxDonor=lastPhotonIdx;
            tStartD=photons.at(lastPhotonIdx).time;
        } else { // indicates error
            burst.startIdxDonor=0;
            burst.startIdxAcceptor=0;
        }
//        t.tock();
//        t2=t2+t.get_duration()*1e6;
//        t.tick();
        lastPhotonIdx=photonIdx;
        if(isDem(photons.at(photonIdx))) {
            burst.durationDonor=photons.at(photonIdx).time-tStartD;
            while(!isAem(photons.at(lastPhotonIdx))) --lastPhotonIdx;
            burst.durationAcceptor=photons.at(lastPhotonIdx).time-tStartA;
        } else if(isAem(photons.at(photonIdx)))  {
            burst.durationAcceptor=photons.at(photonIdx).time-tStartA;
            while(!isDem(photons.at(lastPhotonIdx))) ++lastPhotonIdx;
            burst.durationDonor=photons.at(lastPhotonIdx).time-tStartD;
        } else { // indicates error
            burst.durationDonor=0;
            burst.durationAcceptor=0;
        }
//        t.tock();
//        t3=t3+t.get_duration()*1e6;
#else
        photonIdx=burstSingle.startIdx;
        while(photons.at(photonIdx).time<burst.startTime) ++photonIdx;
        --photonIdx;
        burst.startIdx=photonIdx;
        lastOther=photonIdx;
        if(photons.at(photonIdx).isDem()) {
            burst.startIdxDonor=photonIdx;
            tStartD=photons.at(photonIdx).time;
            while(!photons.at(lastOther).isAem()) ++lastOther;
            burst.startIdxAcceptor=lastOther;
            tStartA=photons.at(lastOther).time;
        } else if(photons.at(photonIdx).isAem())  {
            burst.startIdxAcceptor=photonIdx;
            tStartA=photons.at(photonIdx).time;
            while(!photons.at(lastOther).isDem()) ++lastOther;
            burst.startIdxDonor=lastOther;
            tStartD=photons.at(lastOther).time;
        } else { // indicates error
            burst.startIdxDonor=0;
            burst.startIdxAcceptor=0;
        }
        lastOther=--photonIdx;
        if(photons.at(photonIdx).isDem()) {
            burst.durationDonor=photons.at(photonIdx).time-tStartD;
            while(!photons.at(lastOther).isAem()) --lastOther;
            burst.durationAcceptor=photons.at(lastOther).time-tStartA;
        } else if(photons.at(photonIdx).isAem())  {
            burst.durationAcceptor=photons.at(photonIdx).time-tStartA;
            while(!photons.at(lastOther).isDem()) ++lastOther;
            burst.durationDonor=photons.at(lastOther).time-tStartD;
        } else { // indicates error
            burst.durationDonor=0;
            burst.durationAcceptor=0;
        }
#endif

        COUNTPHOTONS(photons,photonIdx,burst);



        // save burst
        FILTERBURST(burst,BurstSearchParamsGlobal);

        burstVectorDual.push_back(burst);
    }
#ifdef MYDEBUG
    qDebug()<<t1<<t2<<t3;
#endif
    QString result; result=result.sprintf(
                "AllPhotonsBurstSearch  found %d bursts and ignored %i, "
                "of which %i < MinBurstsize=%i, %i > MaxDuration=%.1fms, %i > MaxBurstrate=%.0fkHz "
                "and %i < MinBurstrate=%.0fkHz\n",
                burstVectorDual.size(),
                ignored,ignoredMinBurstsize,BurstSearchParamsGlobal.MinBurstSize,
                ignoredMaxBurstduration, BurstSearchParamsGlobal.MaxDuration*1e3,
                ignoredMaxBurstrate, BurstSearchParamsGlobal.MaxBurstrate*1e-3,
                ignoredMinBurstrate, BurstSearchParamsGlobal.MinBurstrate*1e-3);
    AlexEvalLog::text(result);

    return EXIT_SUCCESS;
}

//#define MYDEBUG
int CombineBurstsOR(QVector<BurstDual> &burstVectorDual, const QVector<Burst> &burstVectorCh1, const QVector<Burst> &burstVectorCh2, const Photons &photons, const BurstSearchParams_t BurstSearchParams )
{
    qDebug("CombineBurstsOR");
#ifdef MYDEBUG
    PublicTickTock tOverlap,tBurst,t,tSave;
    t.tick();
#endif

    // go through burst vector and find overlapping time windows.
    int lastBurstIdx1=0,lastBurstIdx2=0; //last indexed entries of respective burstVector
    int photonIdx=0,lastOther; // last indices of photon vector
    double tEnd1, tEnd2, tEnd;
    double tStart1, tStart2, tStart;
    BurstDual burst;
    Burst burst1, burst2; // bursts of respective burstVector for shorter notation
    burstVectorDual.clear();
    bool ignoreNextCh1=false,ignoreNextCh2=false; // to ignore bursts that have been part of an overlapping burst already
    int ignored=0; // counter for ignored bursts
    int ignoredMinBurstsize=0,ignoredMinBurstsizeDex=0,ignoredMinBurstsizeAex=0, ignoredMinBurstrate=0, ignoredMaxBurstrate=0, ignoredMaxBurstduration=0; // counter for reason of ignoring bursts

    burstVectorDual.push_back(burst); // needed for the first burst, will be removed at the end
    while(lastBurstIdx2<burstVectorCh2.size()&&lastBurstIdx1<burstVectorCh1.size()) { // go through all bursts

        burst1 = burstVectorCh1.at(lastBurstIdx1);
        burst2 = burstVectorCh2.at(lastBurstIdx2);
#ifdef MYDEBUG
        qDebug("lastBurstIdx1=%i lastBurstIdx2=%i", lastBurstIdx1,lastBurstIdx2);
        tOverlap.tick();
#endif
        if(ignoreNextCh1) {
#ifdef MYDEBUG
            qDebug("burst1 was already part of an overlapping burst");
#endif
            ignoreNextCh1=false;
            lastBurstIdx1++;
            continue;
        }
        if(ignoreNextCh2) {
#ifdef MYDEBUG
            qDebug("burst2 was already part of an overlapping burst");
#endif
            ignoreNextCh2=false;
            lastBurstIdx2++;
            continue;
        }
        tStart1 = burst1.startTime;
        tStart2 = burst2.startTime;
        tEnd1 = burst1.startTime + burst1.duration;
        tEnd2 = burst2.startTime + burst2.duration;
        burst.reset();


        if(tEnd1<tStart2) // burst1 is before burst2, no overlap. save donor burst and advance one burst in donor channel.
        {
            lastBurstIdx1++;
#ifdef MYDEBUG
            qDebug("Ch1 burst detected");
#endif
//            if(burstVectorDual.last().startTime>=burstD.startTime) continue; // then the burst was already part of an overlapping burst and must not be saved as a single burst again

            burst.type=Donly;
            burst.startTime = burst1.startTime;
            tEnd = burst1.startTime + burst1.duration;
            burst.duration=burst1.duration;
            burst.durationDonor=burst1.duration;
            photonIdx=qMax(photonIdx,burst1.startIdx);
        }
        else if(tEnd2<tStart1) // burst2 is before burst1, no overlap. Save acceptor burst.
        {
            lastBurstIdx2++;
#ifdef MYDEBUG
            qDebug("Ch2 burst detected");
#endif
            burst.type=Aonly;
            burst.startTime = burst2.startTime;
            tEnd = burst2.startTime + burst2.duration;
            burst.duration=burst2.duration;
            burst.durationAcceptor=burst2.duration;
            photonIdx=qMax(photonIdx,burst2.startIdx);
        }
        else  // else there is an overlap
        {
#ifdef MYDEBUG
            qDebug("overlap detected");
#endif
            burst.type=Dual;
            // find burst start
            if(tStart1<tStart2) tStart = tStart1; // burst1 starts earlier. The dual burst start time is then the start time of the __earlier__ burst.
            else tStart = tStart2;

            photonIdx=qMax(photonIdx,qMin(burst1.startIdx,burst2.startIdx));

            // calculate burst duration
            if(tEnd1<tEnd2) { // burst1 ends earlier. stay at the longer burst(i.e. burst2) and look if there is another overlapping burst in channel 1
                tEnd = tEnd2;
                lastBurstIdx1++; ignoreNextCh2=true;
            }
            else {
                tEnd = tEnd1;
                lastBurstIdx2++; ignoreNextCh1=true;
            }
            while((lastBurstIdx2<burstVectorCh2.size()) && (lastBurstIdx1<burstVectorCh1.size()) ) {
                burst1 = burstVectorCh1.at(lastBurstIdx1);
                burst2 = burstVectorCh2.at(lastBurstIdx2);
                tEnd1 = burst1.endTime();
                tEnd2 = burst2.endTime();
                if(burst1.startTime<tEnd) { // burst1 overlaps with the dual burst.
                    tEnd = qMax(tEnd1,tEnd);
                    lastBurstIdx1++; ignoreNextCh2=true; ignoreNextCh1=false;
                }
                else if (burst2.startTime<tEnd) { // burst2 overlaps with the dual burst.
                    tEnd = qMax(tEnd2,tEnd);
                    lastBurstIdx2++; ignoreNextCh1=true; ignoreNextCh2=false;
                }
                else break;
            }

            burst.startTime = tStart;
            burst.duration = tEnd - tStart;
            burst.durationAcceptor=burst2.duration;
            burst.durationDonor=burst1.duration;
        }
#ifdef MYDEBUG
        tOverlap.tockAvg();
        tBurst.tick();
#endif

        FINDSTARTIDX(photons,photonIdx,burst)



        COUNTPHOTONS(photons,photonIdx,burst);

#ifdef MYDEBUG
        tBurst.tockAvg();
        tSave.tick();
#endif

        FILTERBURST(burst,BurstSearchParams);


        burstVectorDual.push_back(burst); // save burst


#ifdef MYDEBUG
        qDebug()<<"item no."<<burstVectorDual.size();
        qDebug()<<burst.toString().c_str()<<"\n";
        tSave.tockAvg();
        if(burstVectorDual.size()>20) break;
#endif
    }
#ifdef MYDEBUG
    t.tockPrint("print");
    tOverlap.tockAvgPrint("tOverlap");
    tBurst.tockAvgPrint("tBurst");
    tSave.tockAvgPrint("tSave");
#endif
#ifndef MYDEBUG

    if(ignoreNextCh2) lastBurstIdx2++;
    while(lastBurstIdx2<burstVectorCh2.size()) {
        burst2 = burstVectorCh2.at(lastBurstIdx2);
        ++lastBurstIdx2;
        if(ignoreNextCh2) {
            ignoreNextCh2=false;
            continue;
        }
        burst.reset();
        burst.type=Aonly;
        burst.startTime = burst2.startTime;
        burst.duration=burst2.duration;
        burst.durationAcceptor=burst2.duration;
        photonIdx=qMax(photonIdx,burst2.startIdx);

        FINDSTARTIDX(photons,photonIdx,burst);

        COUNTPHOTONS(photons,photonIdx,burst)

        FILTERBURST(burst,BurstSearchParams);

        burstVectorDual.push_back(burst); // save burst
    }


    if(ignoreNextCh1) lastBurstIdx1++;
    while(lastBurstIdx1<burstVectorCh1.size()) {
        burst1 = burstVectorCh1.at(lastBurstIdx1);
        lastBurstIdx1++;
        burst.reset();
        burst.type=Donly;
        burst.startTime = burst1.startTime;
        burst.duration=burst1.duration;
        burst.durationDonor=burst1.duration;
        photonIdx=qMax(photonIdx,burst1.startIdx);

        FINDSTARTIDX(photons,photonIdx,burst);

        COUNTPHOTONS(photons,photonIdx,burst);

        FILTERBURST(burst,BurstSearchParams);

        burstVectorDual.push_back(burst); // save burst

    }

#endif
#ifdef MYDEBUG
    t.tockPrint("total");
    t.tick();
#endif

    ignored=ignoredMinBurstsize+ignoredMaxBurstduration+ignoredMaxBurstrate+ignoredMinBurstrate+ignoredMinBurstsizeDex+ignoredMinBurstsizeAex;

    burstVectorDual.pop_front(); // remove the front burst that contained no information
    QString result; result=result.sprintf(
                "CombineBurstSearch  found %d bursts (of %i D and %i A) and ignored %i, "
                "of which %i < MinBurstsize=%i, %i < ignoredMinBurstsizeDex=%i, %i < ignoredMinBurstsizeAex=%i,\n%i > MaxDuration=%.1fms, %i > MaxBurstrate=%.0fkHz "
                "and %i < MinBurstrate=%.0fkHz\n",
                burstVectorDual.size(),burstVectorCh1.size(),burstVectorCh2.size(),
                ignored,ignoredMinBurstsize,BurstSearchParams.MinBurstSize,
                ignoredMinBurstsizeDex,BurstSearchParams.MinBurstSizeDex,
                ignoredMinBurstsizeAex,BurstSearchParams.MinBurstSizeAex,
                ignoredMaxBurstduration, BurstSearchParams.MaxDuration*1e3,
                ignoredMaxBurstrate, BurstSearchParams.MaxBurstrate*1e-3,
                ignoredMinBurstrate, BurstSearchParams.MinBurstrate*1e-3);
    AlexEvalLog::text(result);



    return EXIT_SUCCESS;
}


int BurstSearchSingle(QVector<Burst> &burstVector, const QVector<double> &photonArrivalTimes, const BurstSearchParams_t &BurstSearchParams)
{
    AlexEvalLog::text("BurstSearchSingle");
    int result=EXIT_FAILURE;
    if(burstVector.size()>0) {
            AlexEvalLog::warning("Warning: overwriting data");
            burstVector.clear();
        }
    if (BurstSearchParams.burstSearchMethodSingle==BurstSearchParams_t::LeeFilter) {
        result=LeeFilterBurstSearch(burstVector,photonArrivalTimes,BurstSearchParams);
    } else if (BurstSearchParams.burstSearchMethodSingle==BurstSearchParams_t::Sliding){
        result=SlidingBurstSearch(burstVector, photonArrivalTimes, BurstSearchParams);
    } else if (BurstSearchParams.burstSearchMethodSingle==BurstSearchParams_t::Binned){
        result=BinnedBurstSearch(burstVector, photonArrivalTimes, BurstSearchParams.T, BurstSearchParams.M, BurstSearchParams.MinBurstSize );
    } else {
        AlexEvalLog::warning("unknown burstSearchMethodSingle");
    }
    return result;
}



int SlidingBurstSearch(QVector<Burst> &burstVector, const QVector<double> &photonArrivalTimes, const BurstSearchParams_t &BurstSearchParams)
{
    qDebug("# Sliding Burst Search with %i photons",photonArrivalTimes.size());
    int left_m, right_m;
    int ii;
    int M = BurstSearchParams.M;
    int L = BurstSearchParams.L();
    double T = BurstSearchParams.T;
    Burst burst;

    int ignored=0; // counter for ignored bursts
    int ignoredMinBurstsize=0, ignoredMinBurstrate=0, ignoredMaxBurstrate=0, ignoredMaxBurstduration=0; // counter for reason of ignoring bursts


    qDebug("parameters are: M=%i, L=%i, T=%.2f us", M, L, T*1e6);
    burst.startIdx=0;

    for(ii=0;ii<photonArrivalTimes.size();++ii) {

        left_m=0;
        right_m=0;

        long left_search_max=qMin(ii, M);
        for (int m=1;m<=left_search_max;m++) { // left neighbours
            if (photonArrivalTimes[ii]-photonArrivalTimes[ii-m]>T/2.0) break; // evaluates to true for first photon outside the left side of the time window
            left_m=m;
        }

        long right_search_max=qMin(photonArrivalTimes.size()-(ii+1), M);
        for (int m=1;m<=right_search_max;m++) { // right neighbours
            if (photonArrivalTimes[ii+m]-photonArrivalTimes[ii]>T/2.0) break;
            right_m=m;
        }
        int m=left_m+right_m; // total number of neighbours
        if(m>=M) { // enough neighbours found
            if(burst.startIdx==0) { // startIdx of a possible burst
                burst.startIdx=ii;
                burst.startTime=photonArrivalTimes[ii];
            }
            continue;
        } else if(burst.startIdx>0 && (ii-burst.startIdx)>=L) { // end of a burst reached. Now ii ist the first photon outside the burst.
            burst.numberOfPhotons=ii-burst.startIdx;
            burst.duration=photonArrivalTimes[ii-1]-burst.startTime;
            if( burst.numberOfPhotons<BurstSearchParams.MinBurstSize) {
                ++ignored;
                ++ignoredMinBurstsize;
                burst.startIdx=0; // reinitialize
                continue;
            }
            if( burst.duration>BurstSearchParams.MaxDuration) {
                ++ignored;
                ++ignoredMaxBurstduration;
                burst.startIdx=0; // reinitialize
                continue;
            }
            if((burst.burstrate())>BurstSearchParams.MaxBurstrate) {
                ++ignored;
                ++ignoredMaxBurstrate;
                burst.startIdx=0; // reinitialize
                continue;
            }
            if((burst.burstrate())<BurstSearchParams.MinBurstrate) {
                ++ignored;
                ++ignoredMinBurstrate;
                burst.startIdx=0; // reinitialize
                continue;
            }
            burstVector.push_back(burst);
            // else ignore burst

            // reinitialize
            burst.startIdx=0;

        } else { // not enough successive photons
            burst.startIdx=0;
        }
    }

    QString result; result=result.sprintf(
                "# Sliding Burst Search found %i bursts and ignored %i, "
                "of which %i < MinBurstsize=%i, %i > MaxDuration=%.1fms, %i > MaxBurstrate=%.0fkHz "
                "and %i < MinBurstrate=%.0fkHz\nParameters were: M=%i, L=%i, T=%.2f us",
                burstVector.size(),ignored,ignoredMinBurstsize,BurstSearchParams.MinBurstSize,
                ignoredMaxBurstduration, BurstSearchParams.MaxDuration*1e3,
                ignoredMaxBurstrate, BurstSearchParams.MaxBurstrate*1e-3,
                ignoredMinBurstrate, BurstSearchParams.MinBurstrate*1e-3,
                M, L, T*1e6);
    AlexEvalLog::text(result);


    return 0;
}




int LeeFilterBurstSearch(QVector<Burst> &burstVector, const QVector<double> &photonArrivalTimes, const BurstSearchParams_t &BurstSearchParams )
{

    QVector<double> interPhotonTime;
    double iptLeeSum;
    double iptLeeVar;
    int winSize=BurstSearchParams.LeeWinSize;
    double leeVar0=BurstSearchParams.LeeVar;

    QString startParams; startParams=startParams.sprintf(
                "Lee Filter Burst Search on %i photons with winsize=%i and variance=%fms^2",
                photonArrivalTimes.size(),winSize,leeVar0*1e6);
    AlexEvalLog::text(startParams);

    for (int j=0;j<photonArrivalTimes.size()-1;j++) { // calculate interphoton times
        interPhotonTime.push_back(photonArrivalTimes[j+1]-photonArrivalTimes[j]);
    }

    for (int i=2*winSize; i<photonArrivalTimes.size()-2*winSize; ++i) { // implement this as recursive filter
        iptLeeSum=0;
        iptLeeVar=0;
        for (int j=(i-winSize+1);j<(i+winSize+1);j++ ) {
            iptLeeSum+=interPhotonTime[j];
        }
        iptLeeSum/=(2*winSize+1);
        for (int j=(i-winSize); j<(i+winSize+1); j++) {
            iptLeeVar += (interPhotonTime[j]-iptLeeSum)*(interPhotonTime[j]-iptLeeSum);
        }
        iptLeeVar= iptLeeVar/((2*winSize)+1);
        interPhotonTime[i]=iptLeeSum+(interPhotonTime[i]-iptLeeSum)/(1+((leeVar0/iptLeeVar)*(leeVar0/iptLeeVar)));
    }


    double MaxIPT=BurstSearchParams.MaxIPT;
    int ignoredMinBurstrate=0;
    int ignoredMaxBurstrate=0;
    int ignoredMaxBurstduration=0;
    int ignored=0;
    int MinBurstSize=BurstSearchParams.MinBurstSize;

    Burst burst;

//    qDebug("Parameters are: minIPT=%.2fus  min burstrate=%.2fkHz  max burstrate=%.2fkHz  max duration=%.1fms  min burstsize=%i photons",MaxIPT*1e6,MinBurstrate*1e-3,MaxBurstrate*1e-3, MaxDuration*1e3, MinBurstSize);
    burst.numberOfPhotons=0;

    for (int j=0;j<photonArrivalTimes.size()-1;j++) {
        if (interPhotonTime.at(j) <= MaxIPT) burst.numberOfPhotons++;
        else {
            if (burst.numberOfPhotons >= MinBurstSize) {// end of the burst is reached

                /*
                for (int p = (j-nPhotons); p< j;p++) {
                    if (router[p]=0)
                        inc(nphoton0[nburst]);
                    else
                        inc(nphoton1[nburst]);
                }
                fret[nburst] := (nphoton1[nburst]-(bred+direx)*burstdur[nburst]-alphacross*(nphoton0[nburst]-bgreen*burstdur[nburst]))/((nphoton1[nburst]-(bred+direx)*burstdur[nburst]-alphacross*(nphoton0[nburst]-bgreen*burstdur[nburst]))+(nphoton0[nburst]-bgreen*burstdur[nburst]));
*/

                burst.startIdx=j-burst.numberOfPhotons;
//                burst.startTime = (photonArrivalTimes[j-1]+photonArrivalTimes[j-burst.numberOfPhotons])/2; // {### arrival time : center of burst ###}
                burst.startTime = photonArrivalTimes[j-burst.numberOfPhotons];
                burst.duration = photonArrivalTimes[j-1]-photonArrivalTimes[j-burst.numberOfPhotons];

                if( burst.duration>BurstSearchParams.MaxDuration) {
                    ++ignored;
                    ++ignoredMaxBurstduration;
                    burst.numberOfPhotons=0; // reinitialize
                    continue;
                }
                if((burst.burstrate())>BurstSearchParams.MaxBurstrate) {
                    ++ignored;
                    ++ignoredMaxBurstrate;
                    burst.numberOfPhotons=0; // reinitialize
                    continue;
                }
                if((burst.burstrate())<BurstSearchParams.MinBurstrate) {
                    ++ignored;
                    ++ignoredMinBurstrate;
                    burst.numberOfPhotons=0; // reinitialize
                    continue;
                }
                burstVector.push_back(burst);

            }
            burst.numberOfPhotons=1; // reinitialize
        }

    }

    QString result; result=result.sprintf(
                "found %i bursts and ignored %i, "
                "of which %i > MaxDuration, %i > MaxBurstrate "
                "and %i < MinBurstrate\n",
                burstVector.size(),ignored,ignoredMaxBurstduration,
                ignoredMaxBurstrate, ignoredMinBurstrate);
    AlexEvalLog::text(result);

    return 0;
}



void burstIntensity(const BurstDual &burst, const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor)
{
    double binwidth=10e-6;
    int nbins=(int)(burst.duration/binwidth+1);
    gsl_histogram * histDonor = gsl_histogram_alloc (nbins); // donor emission channel
    gsl_histogram * histAcceptor = gsl_histogram_alloc (nbins); // acceptor emission channel
    gsl_histogram_set_ranges_uniform (histDonor, 0, burst.duration );
    gsl_histogram_set_ranges_uniform (histAcceptor, 0, burst.duration );

    for(int i=burst.startIdxDonor; i<burst.endIdxDonor();++i) {
        gsl_histogram_increment (histDonor, arrivalTimeDonor[i]-burst.startTime);
    }
    for(int i=burst.startIdxAcceptor; i<burst.endIdxAcceptor();++i) {
        gsl_histogram_increment (histAcceptor, arrivalTimeAcceptor[i]-burst.startTime);
    }

    qDebug("t in s\tDonor emission\tAcceptor emission\ttotal emission");
    for (size_t i=0; i<histDonor->n; ++i) {
        qDebug("%lg\t%lg\t%lg\t%lg", histDonor->range[i], histDonor->bin[i],histAcceptor->bin[i],histDonor->bin[i]+histAcceptor->bin[i]);
    }

    gsl_histogram_free(histDonor);
    gsl_histogram_free(histAcceptor);

}




void burstIntensity(const BurstDual &burst, const QVector<double> &arrivalTimeDonor, const QVector<double> &arrivalTimeAcceptor, const QVector<double> &markerTimeDexc,const QVector<double> &markerTimeAexc )
{
    qDebug("burstIntensity");
    double binwidth=10e-6;
    int nbins=(int)(burst.duration/binwidth+1);
    gsl_histogram * histDonorEm = gsl_histogram_alloc (nbins); // donor emission channel
    gsl_histogram * histAcceptorEm = gsl_histogram_alloc (nbins); // acceptor emission channel
    gsl_histogram * histDonorExc = gsl_histogram_alloc (nbins); // donor excitation channel
    gsl_histogram * histAcceptorExc = gsl_histogram_alloc (nbins); // donor excitation channel
    gsl_histogram_set_ranges_uniform (histDonorEm, 0, burst.duration );
    gsl_histogram_set_ranges_uniform (histAcceptorEm, 0, burst.duration );
    gsl_histogram_set_ranges_uniform (histDonorExc, 0, burst.duration );
    gsl_histogram_set_ranges_uniform (histAcceptorExc, 0, burst.duration );

    for(int i=burst.startIdxDonor; i<burst.endIdxDonor();++i) {
        gsl_histogram_increment (histDonorEm, arrivalTimeDonor[i]-burst.startTime);
    }
    for(int i=burst.startIdxAcceptor; i<burst.endIdxAcceptor();++i) {
        gsl_histogram_increment (histAcceptorEm, arrivalTimeAcceptor[i]-burst.startTime);
    }
    int markeridx=0;

    while(markerTimeDexc.value(markeridx,burst.startTime)<burst.startTime) {markeridx++;}
    qDebug()<<"markeridx"<<markeridx;
    int firstMarker=markeridx;
    while((markerTimeDexc.value(markeridx,1.0)<burst.endTime())&&(markeridx<markerTimeDexc.size())) {
        gsl_histogram_increment (histDonorExc, markerTimeDexc[markeridx]-burst.startTime);
        markeridx++;
    }
    markeridx=firstMarker-1;
    while((markerTimeAexc.value(markeridx,1.0)<burst.endTime())) {
        gsl_histogram_increment (histAcceptorExc, markerTimeAexc[markeridx]-burst.startTime);
        markeridx++;
    }

    qDebug()<<"burst.startIdxDonor="<<burst.startIdxDonor;
    qDebug()<<"burst.startIdxAcceptor="<<burst.startIdxAcceptor;
    qDebug()<<"burst.n_Aexc_Aem="<<burst.n_Aem_Aex;
    qDebug()<<"burst.n_Dexc_Dem="<<burst.n_Dem_Dex;
    qDebug()<<"burst.n_Aexc_Dem="<<burst.n_Dem_Aex;
    qDebug()<<"burst.n_Dexc_Aem="<<burst.n_Aem_Dex;
    qDebug()<<"burst.numberOfPhotons="<<burst.numberOfPhotons;
    qDebug()<<"sum "<<(burst.n_Dem_Dex + burst.n_Aem_Dex + burst.n_Dem_Aex + burst.n_Aem_Aex);
    qDebug("t in ms\tDonor emission\tAcceptor emission\ttotal emission\tDexc\tAexc");
    for (size_t i=0; i<histDonorEm->n; ++i) {
        qDebug("%lg\t%lg\t%lg\t%lg\t%lg\t%lg", histDonorEm->range[i]*1e3, histDonorEm->bin[i],histAcceptorEm->bin[i],histDonorEm->bin[i]+histAcceptorEm->bin[i],histDonorExc->bin[i],histAcceptorExc->bin[i]);
    }

    gsl_histogram_free(histDonorEm);
    gsl_histogram_free(histAcceptorEm);
    gsl_histogram_free(histDonorExc);
    gsl_histogram_free(histAcceptorExc);
}


int BinnedBurstSearch(QVector<Burst> &burstVector, const QVector<double> &photonArrivalTimes, const double binwidth, const int threshold, const int MinBurstSize)
{
    QString startParams; startParams=startParams.sprintf(
                "BinnedBurstSearch with MinBurstSize=%i in a bin of binwidth=%f",
                MinBurstSize,binwidth);
    AlexEvalLog::text(startParams);

    Burst burst;
    burst.startIdx=0;
    double duration = photonArrivalTimes.last()-photonArrivalTimes.first();
    int nPhotons;
    int nbins=(int)(duration/binwidth+0.5);
    if(nbins>1e9) {
        AlexEvalLog::warning("intensity binning might be too big for memory. Aborting...");
        return EXIT_FAILURE;
    }
    gsl_histogram * histEm = gsl_histogram_alloc (nbins); // emission channel
    gsl_histogram_set_ranges_uniform (histEm, photonArrivalTimes.first(), duration+photonArrivalTimes.last());

    for(int i=0; i<photonArrivalTimes.size();++i) {
        gsl_histogram_increment (histEm, photonArrivalTimes.at(i));
    }

    size_t j=0;
    while(j<histEm->n) {
        if(histEm->bin[j]>=threshold) {
            burst.startTime=histEm->range[j];
            nPhotons=0;
            while((j<(histEm->n-1))&&((histEm->bin[j]>=threshold)||(histEm->bin[j+1]>=threshold))) {
                nPhotons+=histEm->bin[j];
                ++j;
            }
            if(nPhotons>MinBurstSize) {
                burst.duration=histEm->range[j]-burst.startTime;
                burst.numberOfPhotons=nPhotons;
                burstVector.append(burst);
            } else {
                ++j;
            }
        } else {
            ++j;
        }
    }
    gsl_histogram_free(histEm);

    QString result; result=result.sprintf(
                "found %i bursts\n", burstVector.size());
    AlexEvalLog::text(result);

    return EXIT_SUCCESS;
}


int inline countPhotons(BurstDual &burst, int &lastD, int &lastA, int &markeridx, const QVector<double> &arrivalTimeDonor,const QVector<double> &arrivalTimeAcceptor, const QVector<double> &markerTimeDexc,  const QVector<double> &markerTimeAexc)
{
   int lastMarker=markeridx;
    bool setperiod;
//        if (markeridx+1>=markerTimeDexc.size()) {AlexEvalLog::warning("not enough marker entries (at the beginning or at the end)");break;}

    // donor emission
//#ifdef MYDEBUG
//            qDebug()<<"lastD="<<lastD;
//#endif
//        while (arrivalTimeDonor.at(lastD)>burst.startTime) --lastD;
//#ifdef MYDEBUG
//            qDebug()<<"lastD="<<lastD;
//#endif
    while (arrivalTimeDonor[lastD]<burst.startTime) ++lastD;
    burst.startIdxDonor=lastD;
    while(markerTimeDexc[markeridx]>arrivalTimeDonor.at(burst.startIdxDonor)) --markeridx;
    while((markerTimeDexc[markeridx]<burst.startTime)) ++markeridx; // skip the first markers until the burst begins
    // (markeridx >= markerTimeDexc.size()) ||
    --markeridx; // go one marker back to be ahead of the burst with the marker
    if (markeridx>=markerTimeDexc.size()) {AlexEvalLog::warning("not enough marker entries (at the beginning or at the end)");return EXIT_FAILURE;}
    lastMarker=markeridx; // save marker index

    while(arrivalTimeDonor[lastD]<=burst.endTime()) { // count the number of photons of the new burst in the first channel.
        ++burst.numberOfPhotons;
        setperiod=false;
        while (!setperiod) {
            if ((markerTimeDexc[markeridx]<arrivalTimeDonor[lastD] )&& (arrivalTimeDonor[lastD]<=markerTimeAexc[markeridx])) { // note that the makro time of two events can indeed be the same. thus the "smaller or equal" operator is vital!
                ++burst.n_Dem_Dex; // then it is in the donor excitation period
                setperiod=true;
            } else if ((markerTimeAexc[markeridx]<arrivalTimeDonor[lastD]) && (arrivalTimeDonor[lastD]<=markerTimeDexc[markeridx+1])) {
                ++burst.n_Dem_Aex;
                setperiod=true;
            } else ++markeridx; // try next alternation period
        }
        lastD++;
    }


    // acceptor emission
//#ifdef MYDEBUG
//            qDebug()<<"lastA="<<lastA;
//#endif
//        while (arrivalTimeAcceptor.at(lastA)>burst.startTime) --lastA;
//#ifdef MYDEBUG
//            qDebug()<<"lastA="<<lastA;
//#endif
    while (arrivalTimeAcceptor.at(lastA)<burst.startTime) ++lastA;
    burst.startIdxAcceptor=lastA;
    markeridx=lastMarker; // reset marker index
    while(markerTimeDexc[markeridx]>arrivalTimeAcceptor[burst.startIdxAcceptor]) --markeridx;
    while(markerTimeDexc[markeridx]<burst.startTime) ++markeridx; // skip the markers until the burst begins
    --markeridx;
    if (markeridx>=markerTimeAexc.size()) {AlexEvalLog::warning("not enough marker entries (at the beginning or at the end)");return EXIT_FAILURE;}
    lastMarker=markeridx; // save marker

    while(arrivalTimeAcceptor[lastA]<=burst.endTime()) {
        ++burst.numberOfPhotons;
        setperiod=false;
        while (!setperiod) {
            if ( (markerTimeDexc[markeridx]<arrivalTimeAcceptor[lastA]) && (arrivalTimeAcceptor[lastA]<=markerTimeAexc[markeridx])) {
                ++burst.n_Aem_Dex; // then it is in the donor excitation period
                setperiod=true;
            } else if ((markerTimeAexc[markeridx]<arrivalTimeAcceptor[lastA]) && (arrivalTimeAcceptor[lastA]<=markerTimeDexc[markeridx+1])) {
                ++burst.n_Aem_Aex;
                setperiod=true;
            } else ++markeridx; // try next alternation period
        }
        lastA++;
    }

    return EXIT_SUCCESS;

}
