/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), Sebastian Isbaner
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

#ifndef TTTRTOOLS_H
#define TTTRTOOLS_H

#include "qftools.h"
#include "qfmathtools.h"
#include <cmath>
#include "statistics_tools.h"
#include <QVector>
#include <stdint.h>

/** \group qf3_tttr_tools TTTR Tools
 *  \ingroup qf3tools
 */


/**
 * \brief crosscorrelateTTTR crosscorrelates single arrival times of two channels
 * \ingroup qf3_tttr_tools
 *
 * \param t arrival times in channel 1 with Nt points
 * \param Nt number of data points in channel t
 * \param u arrival times in channel 2 with Nu points
 * \param Nu number of data points in channel u
 * \param g pointer to the correlation function array (call by reference) containing at least ntau entries
 * \param tau pointer to the lag times that should be used to calculate the correlation function
 * \param ntau number of discrete lag times
 */
template<typename TDATA, typename TCORR>
inline void TTTRcrosscorrelate(const TDATA *t, int64_t Nt, const TDATA *u, int64_t Nu, TCORR *g, const TCORR *tau, const uint ntau) {

    TCORR tStart=qMin(t[0],u[0]);
    TCORR tEnd = qMax(t[Nt-1],u[Nu-1]);
    TCORR T = tEnd - tStart; // total acquisition time
    uint64_t *lk=qfMallocT<uint64_t>(ntau); // photon indices
    uint64_t *mk=qfMallocT<uint64_t>(ntau); // photon indices
    TCORR *nt=qfMallocT<TCORR>(ntau); // photon count in channel t
    TCORR *nu=qfMallocT<TCORR>(ntau); // photon count in channel u

    //    (1) Initialize a correlogram Yk with M bins to 0.
    for(uint k=0;k<ntau;++k) {
        g[k]=0;
        lk[k]=1;
        mk[k]=1;
        nt[k]=0;
        nu[k]=0;
    }
    g[ntau-1]=1;


    for(int64_t i=0;i<Nt;++i) {// loop over all photons in channel t
        const TCORR ti=t[i];
        for(register uint k=0;k<ntau-1;++k) { // loop over bins
            const TCORR tauk=tau[k];
            const TCORR taukp1=tau[k+1];

            const TCORR tauStart = ti + tauk;
            const TCORR tauEnd = ti + taukp1;
            register int64_t l=lk[k];
            while( (l<Nu-1) && tauStart>u[l]) { // start photon in bin k
                l++;
            }

            register int64_t m=mk[k];
            while( (m<Nu-1) && u[m]<tauEnd /*! (u[m-1] < tauEnd && tauEnd <= u[m])*/) { // end photon in bin k
                m++;
            }

            //if (l<Nu && m<Nu) {
                lk[k] = l;
                mk[k] = m;
                g[k] = g[k] + TCORR(m-l);  // update correlogram
            //}
        }
    }

    // Normalize

    for(uint k=0;k<ntau-1;++k) { // loop over bins
        int64_t i=0;
        while (i<Nu && u[i]-tStart<tau[k]) {
            i++;
        }
        nu[k]=Nu-i;
        i=Nt-1;
        while (i>1 && u[i]-tStart>T-tau[k]) {
            i--;
        }
        nt[k]=i;
    }

    /*for(int64_t i=0;i<Nu;++i) {// loop over all photons in channel u
        for(int64_t k=0;k<ntau-1;++k) { // loop over bins
            if((u[i]-tStart)>=tau[k]) ++nu[k];
        }
    }*/
    for(uint k=0;k<ntau-1;++k) {
        // normalize by the number of photons for the lag times (symmetrically for u and t)
        // and the binwidth tau_{k+1} - tau_k (rectangular averaging).
        g[k]=g[k]*(T-tau[k])/(tau[k+1]-tau[k])/(TCORR)nt[k]/(TCORR)nu[k];
    }

    if (nt) qfFree(nt);
    if (nu) qfFree(nu);
    if (lk) qfFree(lk);
    if (mk) qfFree(mk);




}





/**
 * \brief correlateTTTR correlates single photon arrival times in one channel
 * \ingroup qf3_tttr_tools
 *
 * \param t arrival times with N points
 * \param N number of data points in channel t
 * \param g pointer to the correlation function array (call by reference) containing at least ntau allocated entries
 * \param tau pointer to an array of lag times that should be used to calculate the correlation function with ntau elemts
 * \param ntau number of discrete lag times
 */
template<typename TDATA, typename TCORR>
inline void TTTRcorrelate(const TDATA *t, const int64_t N, TCORR *g, const TCORR *tau, const uint ntau) {
    TTTRcrosscorrelate(t, N, t, N, g, tau, ntau);
    /*TCORR tDeltaStart, tDeltaEnd;
    TCORR tStart=t[0]; // assuming measurement begins at t[0]
    TCORR T = t[N-1]-t[0]; // total acquisition time assuming measurement begins at t[0]
    const TDATA *u=t;
    int64_t *lk=qfMallocT<int64_t>(ntau); // photon indices
    int64_t *mk=qfMallocT<int64_t>(ntau); // photon indices
    TCORR *nt=qfMallocT<TCORR>(ntau); // photon count in channel t
    TCORR *nu=qfMallocT<TCORR>(ntau); // photon count in channel u

    //    (1) Initialize a correlogram Yk with M bins to 0.
    for(int64_t k=0;k<ntau;++k) {
        g[k]=0;
        lk[k]=1;
        mk[k]=1;
        nt[k]=0;
        nu[k]=0;
    }


    for(int64_t i=0;i<N;++i) {// loop over all photons in channel t

        // the "0th" bin
        lk[0] = i; // for autocorrelation, the indices match
        int64_t l=lk[0];
        tDeltaEnd = t[i] + tau[0];
        int64_t m=mk[0];
        if(m==0) m=1;
        while((u[m-1] >= tDeltaEnd || tDeltaEnd > u[m]) && m<(N-1)) ++m;
        mk[0] = m;
        g[0] = g[0] + TCORR(mk[0] - lk[0]);
        if((t[i]-tStart)<=(T-tau[0])) ++nt[0];
        if((u[i]-tStart)>=tau[0]) ++nu[0];


        for(int64_t k=1;k<ntau;++k) { // loop over bins k>0

            tDeltaStart = t[i] + tau[k-1];

            l=lk[k];
            while(l<N && u[l]<tDeltaStart) ++l;
            lk[k]=l;

            tDeltaEnd = t[i] + tau[k];

            m=mk[k];
            while(m<N && u[m]<tDeltaEnd) ++m;
            mk[k]=m;

//            int m=mk[k]; while((u[m-1] >= tauEnd || tauEnd > u[m]) || m==N) m++; mk[k] = m;

            g[k] = g[k] + TCORR(mk[k] - lk[k]);  // update correlogram

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
        g[k]=g[k]*(T-tau[k])/(tau[k]-tau[k-1])/((TCORR)nt[k]*(TCORR)nu[k]);
    }

    if (nt) qfFree(nt);
    if (nu) qfFree(nu);
    if (lk) qfFree(lk);
    if (mk) qfFree(mk);*/
}

template <typename T, typename T2>
int64_t TTTRcoarseData(T* data, T2* w, int64_t Ndata, T tauMin, T Toffset=T(0)) {
    int64_t N=0;

    for (int64_t i=0; i<Ndata; i++){
        data[i]=floor((data[i]-Toffset)/tauMin)*tauMin;
    }
    int64_t i=0;
    N=0;
    while (i<Ndata) {
        w[N]=w[i];
        const T ti=data[i];
        i++;
        while (i<Ndata && data[i]==ti) {
            w[N]=w[N]+w[i];
            i++;
        }
        data[N]=ti;
        N++;
    }

    return N;
}



/**
 * \brief crosscorrelateTTTR crosscorrelates single arrival times of two channels
 * \ingroup qf3_tttr_tools
 *
 * \param t_in arrival times in channel 1 with Nt points
 * \param Nt number of data points in channel t
 * \param u_in arrival times in channel 2 with Nu points
 * \param Nu number of data points in channel u
 * \param g pointer to the correlation function array (call by reference) containing at least ntau entries
 * \param tau pointer to the lag times that should be used to calculate the correlation function
 * \param ntau number of discrete lag times
 */
template<typename TDATA, typename TCORR>
inline void TTTRcrosscorrelateWithAvg(const TDATA *t_inn, int64_t Ntn, const TDATA *u_inn, int64_t Nun, QVector<TCORR>& g, QVector<TCORR>& tau, int S, int m, int P, double tauMin) {
    const TDATA *t_in=t_inn;
    int64_t Nt=Ntn;
    const TDATA *u_in=u_inn;
    int64_t Nu=Nun;
    while (Nt>1 && t_in[0]>t_in[1]) {
        t_in=&(t_in[1]);
        Nt--;
    }
    while (Nu>1 && u_in[0]>u_in[1]) {
        u_in=&(u_in[1]);
        Nu--;
    }

    //TDATA* t=qfDuplicateArray(t_in, Nt);
    //TDATA* uu=qfDuplicateArray(u_in, Nu);
    int32_t* wt=qfMallocT<int32_t>(Nt);
    int32_t* wu=qfMallocT<int32_t>(Nu);
    int64_t* t=qfMallocT<int64_t>(Nt);
    int64_t* uu=qfMallocT<int64_t>(Nu);
    TCORR tStart=qMin(t_in[0],u_in[0]);
    TCORR tEnd = qMax(t_in[Nt-1],u_in[Nu-1]);
    TCORR T = tEnd - tStart; // total acquisition time
    qDebug()<<"TTTRcrosscorrelateWithAvg: tStart="<<tStart<<"  tEnd="<<tEnd<<"  T="<<T<<"  S="<<S<<"  P="<<P<<"  m="<<m<<"  tauMin="<<tauMin;
    int ntau=S*P;
    int gi=0;

    for (int64_t i=0; i<Nt; i++) {
        wt[i]=1;
        t[i]=floor(t_in[i]/tauMin);
    }
    for (int64_t i=0; i<Nu; i++) {
        wu[i]=1;
        uu[i]=floor(u_in[i]/tauMin);
    }

    int64_t Ntt=Nt, Nuu=Nu;

    // count photons after tau and before T-tau
    TCORR N1all=Ntt;
    TCORR N2all=Nuu;


    Ntt=TTTRcoarseData(t, wt, Ntt, (int64_t)1);
    Nuu=TTTRcoarseData(uu, wu, Nuu, (int64_t)1);
    //Ntt=TTTRcoarseData(t, wt, Ntt, tauMin);
    qDebug()<<"Ntt="<<Ntt<<Nt<<t[0]<<"("<<wt[0]<<t_in[0]<<"), "<<t[1]<<"("<<wt[1]<<t_in[1]<<"), "<<t[2]<<"("<<wt[2]<<t_in[2]<<"), "<<t[3]<<"("<<wt[3]<<t_in[3]<<")";
    //Nuu=TTTRcoarseData(uu, wu, Nuu, tauMin);
    qDebug()<<"Nuu="<<Nuu<<Nu<<uu[0]<<"("<<wu[0]<<u_in[0]<<"), "<<uu[1]<<"("<<wu[1]<<u_in[1]<<"), "<<uu[2]<<"("<<wu[2]<<u_in[2]<<"), "<<uu[3]<<"("<<wu[3]<<u_in[3]<<")";
    //TDATA* u=qfDuplicateArray(uu, Nuu);
    int64_t* u=qfDuplicateArray(uu, Nuu);
    int64_t lasttau=0;
    tau.clear();
    g.clear();

    for (int s=0; s<S; s++) {
        for (int p=0; p<P; p++) {
            if (s==0 || (s>0 && p>=P/m)) {
                if (gi==0) { lasttau=1; tau.append(TCORR(lasttau)*tauMin); }
                else { lasttau=lasttau+pow(m, s); tau.append(TCORR(lasttau)*tauMin); }
                g.append(0);
                qDebug()<<gi<<s<<p<<lasttau<<tau[gi];
                register int64_t pcnt=0, ti=0, ui=0;

                int64_t in=Nu-1;
                while (in>0 && u_in[in]>tEnd-tau[gi]) {
                    in--;
                }
                TCORR N2tau=in;
                in=0;
                while (in<Nt-1 && u_in[in]<tau[gi]) {
                    in++;
                }
                TCORR N1tau=Nt-in;

                // generate array {u1'=u1+tau, u2'=u2+tau, ...}
                for (int64_t i=0; i<Nuu; i++) {
                    u[i]=uu[i]+lasttau;
                }





                // count photon-pairs with distance lasttau ... tau[gi]
                while (ti<Ntt-1 && ui<Nuu-1) {

                    //TDATA dt=0;
                    while (ti<Ntt-1 && t[ti]<u[ui]) {
                        //dt=u[ui]-t[ti];
                        //if (dt<tau[gi] && dt>lasttau) pcnt+=(wt[ti]*wu[ui]);
                        ti++;
                    }
                    if (u[ui]==t[ti]) pcnt+=(wt[ti]*wu[ui]);



                    while (ui<Nuu-1 && t[ti]>=u[ui]) {
                        //dt=t[ti]-u[ui];
                        //if (dt<tau[gi] && dt>lasttau) pcnt+=(wt[ti]*wu[ui]);
                        ui++;
                    }
                    if (u[ui]==t[ti]) pcnt+=(wt[ti]*wu[ui]);

                    //qDebug()<<ti<<ui;

                }

                // update correlation
                g[gi]=TCORR(pcnt)/pow(m, s)/(T-tau[gi])/(N1all/T)/(N2tau/(T-tau[gi]));
                qDebug()<<gi<<s<<p<<tau[gi]<<":  "<<pcnt<<pcnt/pow(m, s)<<N1tau*N2tau<<N1tau<<N2tau<<T<<g[gi];

                //lasttau=tau[gi];
                gi++;
            }
        }

        // time-coarsening
        Ntt=TTTRcoarseData(t, wt, Ntt, (int64_t)pow(m, s+1));
        Nuu=TTTRcoarseData(uu, wu, Nuu, (int64_t)pow(m, s+1));
        qDebug()<<"Ntt="<<Ntt<<Nt<<t[0]<<"("<<wt[0]<<t_in[0]<<"), "<<t[1]<<"("<<wt[1]<<t_in[1]<<"), "<<t[2]<<"("<<wt[2]<<t_in[2]<<"), "<<t[3]<<"("<<wt[3]<<t_in[3]<<")";
        int64_t wsum=0;
        for (int64_t jj=0; jj<Ntt; jj++) wsum+=wt[jj];
        qDebug()<<"wtsum="<<wsum;
        qDebug()<<"Nuu="<<Nuu<<Nu<<uu[0]<<"("<<wu[0]<<u_in[0]<<"), "<<uu[1]<<"("<<wu[1]<<u_in[1]<<"), "<<uu[2]<<"("<<wu[2]<<u_in[2]<<"), "<<uu[3]<<"("<<wu[3]<<u_in[3]<<")";
        wsum=0;
        for (int64_t jj=0; jj<Nuu; jj++) wsum+=wu[jj];
        qDebug()<<"wusum="<<wsum;
    }



    qfFree(t);
    qfFree(u);
    qfFree(uu);
    qfFree(wt);
    qfFree(wu);
}

template<typename TDATA, typename TCORR>
inline void TTTRcorrelateWithAvg(const TDATA *t_in, int64_t Nt,QVector<TCORR>&g, QVector<TCORR>&tau, int S, int m, int P, double tauMin) {
    TTTRcrosscorrelateWithAvg(t_in, Nt, t_in, Nt, g, tau, S, m, P, tauMin);
}

#endif // TTTRTOOLS_H
