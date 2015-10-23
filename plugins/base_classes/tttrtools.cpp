/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>),
    German Cancer Research Center/University Heidelberg

    last modification: $LastChangedDate: 2015-01-21 11:37:05 +0100 (Mi, 21 Jan 2015) $  (revision $Rev: 3738 $)

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

#include "tttrtools.h"






bool TTTRReadPhotonsData(TCSPCPhotonsData *out, QFTCSPCReader *readerData, QFTCSPCReader *readerBackground, TTTRReadPhotonsDataEvalProps props)
{
    const double start=props.start;
    const double end=props.end;
    const int chG=props.chG;
    const int chR=props.chR;
    out->backgroundrate.clear();
    out->avgrate.clear();
    out->avg_IPT=0;
    out->backgroundDuration=0;
    out->duration=0;
    out->micrrotime_dt=1;
    out->photonsG=0;
    out->photonsR=0;
    out->lastLEESigma0=0;
    out->lastLEEWindowSize=0;
    if (readerBackground) {
        double dur=0;
        for (int i=0; i<readerBackground->inputChannels(); i++) {
            out->backgroundrate<<0;
        }
        readerBackground->reset();
        const double firstPhoton=readerBackground->getCurrentRecord().absoluteTime();
        do {
            QFTCSPCRecord r=readerBackground->getCurrentRecord();
            if (r.isPhoton) {
                if (r.input_channel<out->backgroundrate.size()) {
                    out->backgroundrate[r.input_channel]++;
                    dur=r.absoluteTime()-firstPhoton;
                }
            }
        } while (readerBackground->nextRecord());
        for (int i=0; i<out->backgroundrate.size(); i++) {
            out->backgroundrate[i]/=dur;
        }
        out->backgroundDuration=dur;
    }
    if (readerData) {
        out->photondata.clear();
        double dur=0;
        readerData->reset();
        for (int i=0; i<readerData->inputChannels(); i++) {
            out->avgrate<<0;
        }

        while (start>=0 && readerData->getCurrentRecord().absoluteTime()<start && readerData->nextRecord()) {

        }
        const double firstPhoton=readerData->getCurrentRecord().absoluteTime();
        QFTCSPCRecord rlast=readerData->getCurrentRecord();
        QFTCSPCRecord r=rlast;
        if (readerData->nextRecord()) {
            do {
                QFTCSPCRecord rtmp=r;
                r=readerData->getCurrentRecord();

                if (r.isPhoton) {
                    rlast=rtmp;
                    dur=r.absoluteTime()-firstPhoton;
                    TCSPCPhotonData p;
                    p.arrivaltime=r.absoluteTime();
                    p.channel=r.input_channel;
                    p.microtime=r.microtime_channel;
                    p.IPT=p.arrivaltime-rlast.absoluteTime();
                    p.IPT_filtered=p.IPT;
                    out->avg_IPT+=p.IPT;
                    out->micrrotime_dt=r.microtime_deltaT;
                    if (p.channel==chG) out->photonsG++;
                    if (p.channel==chR) out->photonsR++;
                    out->photondata.append(p);
                    if (p.channel<out->avgrate.size()) out->avgrate[p.channel]++;
                }
            } while (readerData->nextRecord() && (end<0 || readerData->getCurrentRecord().absoluteTime()<end));
        }
        out->duration=dur;
        out->firstPhoton=firstPhoton;
        out->avg_IPT/=double(out->photondata.size());
        for (int i=0; i<readerData->inputChannels(); i++) {
            out->avgrate[i]/=dur;
        }
        return true;
    }
    return false;
}


void TCSPCPhotonsData::applyLEEFilter(int win, double sigma0)
{
    if (win!=lastLEEWindowSize || sigma0!=lastLEESigma0) {
        lastLEEWindowSize=win;
        lastLEESigma0=sigma0;
        QVector<double> avg(photondata.size(), 0.0);
        for (int i=0; i<photondata.size(); i++) {
            const int jstart=qBound(0, i-win/2, photondata.size()-1);
            const int jend=qBound(0, i+win/2, photondata.size()-1);
            const double phots=jend-jstart+1;
            avg[i]=0;
            for (int j=jstart; j<=jend; j++) {
                avg[i]+=photondata[j].IPT/phots;
            }
        }
        for (int i=0; i<photondata.size(); i++) {
            const int jstart=qBound(0, i-win/2, photondata.size()-1);
            const int jend=qBound(0, jstart+win, photondata.size()-1);
            const double phots=jend-jstart+1;
            double sigma=0;
            for (int j=jstart; j<=jend; j++) {
                sigma+=qfSqr(photondata[j].IPT-avg[j])/phots;
            }
            photondata[i].IPT_filtered=avg[i]+(photondata[i].IPT-avg[i])*sigma/(sigma+qfSqr(sigma0));
        }
    }
}


void TTTRFindBursts(TCSPCBurstsData *output, const TCSPCPhotonsData &photons, TTTRFindBurstsEvalProps props)
{
    output->burstdata.clear();
    output->avgDuration=0;
    bool isBurst=false;
    TCSPCBurstData b;
    for (int i=0; i<photons.photondata.size()-props.minBurstSize; i++) {
        if (isBurst) {
            if ((props.filtered && photons.photondata[i].IPT_filtered>props.maxIPT)
                    ||(!props.filtered && photons.photondata[i].IPT>props.maxIPT)) {
                isBurst=false;
                if (b.duration<=props.maxBurstDuration && b.size>=props.minBurstSize) {
                    output->burstdata.append(b);
                    output->avgDuration+=b.duration;
                }
            } else {
                b.size++;
                b.duration=photons.photondata[i].arrivaltime-b.start;
                if (photons.photondata[i].channel==props.chG) b.photonG++;
                if (photons.photondata[i].channel==props.chR) b.photonR++;
            }
        } else {
            bool startburst=true;
            if (props.filtered) {
                for (int j=i; j<=i+props.minBurstSize; j++) {
                    if (photons.photondata[j].IPT_filtered>props.maxIPT) {
                        startburst=false;
                        break;
                    }
                }
            } else {
                for (int j=i; j<=i+props.minBurstSize; j++) {
                    if (photons.photondata[j].IPT>props.maxIPT) {
                        startburst=false;
                        break;
                    }
                }
            }
            if (startburst) {
                b.start=photons.photondata[i].arrivaltime;
                b.duration=0;
                b.photonG=0;
                b.photonR=0;
                b.size=1;
                b.P=0;
                b.E=0;
                if (photons.photondata[i].channel==props.chG) b.photonG++;
                if (photons.photondata[i].channel==props.chR) b.photonR++;
                isBurst=true;
            }
        }
    }
    output->avgDuration/=double(output->burstdata.size());
}


void TTTRCalcBurstProperties(TCSPCBurstsData *output, TTTRCalcBurstPropertiesProps props)
{
    for (int i=0; i<output->burstdata.size(); i++) {
        double ND=double(output->burstdata[i].photonG)-output->burstdata[i].duration*props.backG;
        double NA=double(output->burstdata[i].photonR)-output->burstdata[i].duration*(props.backR+props.fdirect)-props.crosstalk*ND;
        output->burstdata[i].P=NA/(NA+ND);
        double E=output->burstdata[i].E=NA/(NA+props.gamma*ND);
        output->burstdata[i].rDA=props.R0*pow(1.0/E-1.0, 1.0/6.0);
    }
}
