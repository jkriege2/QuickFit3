/*
    Copyright (c) 2008-2014 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate$  (revision $Rev$)

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



#ifndef MULTITAUCORRELATOR_H
#define MULTITAUCORRELATOR_H

#include "shiftregister.h"
#include <vector>
#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <cmath>

/** \file multitau-correlator.h
 *  \ingroup tools_math_correlator
 */

/*! \brief returns the auto- or crosscorrelation function of the given timeseries by using a multi-tau correlation scheme
   \ingroup tools_math_correlator


  \section MultiTauCorrelator_Notation Notation
  Some Notation for the documentation:
      -# In general we calculate the correlation function of two signals \f$ n_1(t) \f$ and \f$ n_2(t) \f$ :
            \f[ g_{12}(\tau)=\frac{\left\langle n_1(t)\cdot n_2(t+\tau) \right\rangle_t }{\langle n_1\rangle_t\cdot\langle n_2\rangle_t} \f]
         For the special case of a cross correlation function of one signal \f$ n(t) \f$ we set \f$ n_1(t)=n_2(t)=n(t) \f$.
      -# The signals \f$ n(t) \f$ are discrete in time, so \f$ t=1,2,...,T_{\rm max} \f$ , so the average \f$ \langle n(t) \rangle_t \f$ is
         defined in terms of sums: \f[ \langle n \rangle_t=\frac{1}{T_{\rm max}}\cdot\sum\limits_{t=1}^{T_{\rm max}}n(t) \f]
    .


  \section MultiTauCorrelator_Theory Multi-Tau Correlation Theory
  As the Normalization can be done as an independent last step after the correlation, we only have to cope with the calculation
  of \f$ \left\langle n_1(t)\cdot n_2(t+\tau) \right\rangle_t \f$ here.

  \subsection MultiTauCorrelator_Theory_Linear Linear Correlators
  It is quite simple and straight-forward to implement a linear correlator. For this we simply have to calculate:
    \f[ g(\tau)=\left\langle n_1(t)\cdot n_2(t+\tau) \right\rangle_t=\frac{1}{T_{\rm max}}\cdot\sum\limits_{t=1}^{T_{\rm max}}n_1(t)\cdot n_2(t+\tau) \f]
  for several value \f$ \tau=1,2,3,...,P \f$. The software implementation of this is loaned from it's implementation in hardware.
  We use a so called shift register, which is basically a FIFO with a certain length \f$ P \f$ . When \f$ P \f$ values
  \f$ n(P)...n(1) \f$ have been pushed into it, it contains exactly these \f$ P \f$ values. If another value \f$ n(P+1) \f$
  is pushed in, the oldest value \f$ n(1) \f$ falls out of the register and it then contains the \f$ P \f$ values
  \f$ n(P+1)...n(2) \f$ . Initially the register is filled with all 0. Before we push in a new value \f$ n_1(t) \f$ , we multiply all old
  values in the register with the current value \f$ n_2(t) \f$ of the second signal and accumulate the results for
  every entry in the register separately. This way we build up the correlation function \f$ g(\tau) \f$ at the lag times \f$ \tau=1..P \f$ :\verbatim
  input       | register (P=4)          | g(1)                                        | g(2)
--------------+-------------------------+---------------------------------------------+-----------------------------------------
              |     0     0     0     0 | 0                                           | 0
 n1(1), n2(1) | n1(1)     0     0     0 | 0*n2(1)                                     | 0*n2(1)
 n1(2), n2(2) | n1(2) n1(1)     0     0 | 0*n2(1)+n1(1)*n2(2)                         | 0*n2(1)+0*n2(2)
 n1(3), n2(3) | n1(3) n1(2) n1(1)     0 | 0*n2(1)+n1(1)*n2(2)+n1(2)*n2(3)             | 0*n2(1)+0*n2(2)+n1(1)*n2(3)
 n1(4), n2(4) | n1(4) n1(3) n1(2) n1(1) | 0*n2(1)+n1(1)*n2(2)+n1(2)*n2(3)+n1(3)*n2(2) | 0*n2(1)+0*n2(2)+n1(1)*n2(3)+n1(2)*n2(4)
\endverbatim

  \subsection MultiTauCorrelator_Theory_MultiTau Multi-Tau Correlators
  Often one want's to cover several decades of lag times \f$ \tau \f$ , so it is very consuming to implement a very long linear correlator.
  One could also think about evaluating \f$ g(\tau) \f$ only at certain values, say \f$ \tau=1,2,4,8,16, ... \f$ to span several decades,
  but with fewer values. This is basically what a multi-tau correlator does, but in a more involved way. It is constructed of a series of
  \f$ S \f$ linear correlators, where the first correlator is fed with the signals \f$ n_x(t) \f$ directly. The second correlator is fed only
  in every 2nd time step \f$ t=2, 4, 6, ... \f$ , but with the summed up signal \f[ n^{(2)}_x(t)=n_x(t-1)+n_x(t). \f]  Then the third
  correlator is fed in every 4th timestep \f$ t=4, 8, 12, ... \f$, but with the signal \f[ n^{(3)}_x(t)=n_x(t-3)+n_x(t-2)+n_x(t-1)+n_x(t)=n^{(2)}_x(2)+n^{(2)}_x(4). \f]


  If we generalize this to \f$ S \f$ linear correlators with \f$ P \f$ channels each and a factor of \f$ m \f$ ( in the example \f$ m=2 \f$ ) between
  two subsequent push rates, we get:
    \f[ t=m^i,2m^i,3m^i,...: n^{(i)}_x(t)=\sum\limits_{k=1}^{m^i}n_x(t)=\sum\limits_{k=1}^{m^(i-1)}n_x^{(i-1)}(t\cdot m). \f]
  and a quasi-logarithmic evaluation of \f$ g(\tau) \f$ at ( \f$ m \f$ has to divide \f$ P \f$ !):
    \f[ \tau=1,2,...,P, P+m, P+2m, ... P+(P/2)m, P+(P/2)m+m^2, P+(P/2)m+2m^2, ... P+Pm+(P/2)m^2, ...  \f]
  The factor 1/2 stems from the fact that the first \f$ P/2 \f$ channels in each correlator (except the first) are already covered by the preceding
  correlator. If we set  \f$ m=2 \f$ has to divide \f$ P=4 \f$ we get:
    \f[ \tau=1,2,3,4,6,8,12,16,24,32,48,64,...  \f]

  \note This described method incorporates an implicit averaging over the signal for larger lag-times, i.e. later linear correlators.


  \subsection MultiTauCorrelator_Theory_Norm Normalization
  For some applications (e.g. FCS or DLS), in the final step the result has to be normalized, i.e. divided by the average signal(s):
    \f[ g_{12}(\tau)=\frac{\left\langle n_1(t)\cdot n_2(t+\tau) \right\rangle_t }{\langle n_1\rangle_t\cdot\langle n_2\rangle_t}. \f]
  So the question arrises which estimator to use for the averages in the denominator. The simplest method is to simply sum up the input signals:
    \f[ M_{i}=\sum\limits_{t=1}^{T_{\mathrm{max}}} n_{i}(t),\ \ \ \ \ i\in\{1,2\} \f]
  and divide by this sum:
    \f[ g(\tau)=\frac{T_{\mathrm{max}}\cdot A_\tau}{M_1\cdot M_2}\ \ \ \ \ \text{with}\ \ \ \ \ A_\tau=\sum\limits_{t=1}^{T_{\mathrm{max}}}n_1(t)\cdot n_2(t+\tau) \f]
  where \f$ A_\tau \f$ is the result of the correlator for a given \f$ \tau \f$ . This noralization is known to be biased for large lag times, i.e. when
  \f$ \tau \f$ is comparable to \f$ T_{\mathrm{max}} \f$ , if the above multi-\f$\tau\f$ scheme is used, as the later linear correlator blocks do not sum
  over the same amount of data as the first channels. To overcome this effect, the <b>symmetrical normalization</b> is used. Here we have a monitor channel for each
  and every lag time, i.e.:
    \f[ M_{i,\tau}=\sum\limits_{t=1}^{T_{\mathrm{max}}} n_{i}(t-\tau) \f]
  Now the normalization reads:
    \f[ g_{\text{sym}}(\tau)=\frac{T_{\mathrm{max}}}{2^s}\cdot\frac{A_\tau}{M_1\cdot M_{2,\tau}} \f]



  \section MultiTauCorrelator_Programming Programmatical Realisation
  \subsection MultiTauCorrelator_Construction Construction/Definition of Properties
  The build-up of the multi-tau correlator is described in the constructor. There you can specify
  the length of the linear correlators, their overlap ratio and also the number of linear correlators
  making up the multi-tau correlator.

  This class is implemented as a template to allow for different datatypes. There are two template parameters.
  The first is the dataformat of the values supplied to the correlator. The second one is the datatype used
  for the monitor channels. In the simplest case you can use <code>MultiTauCorrelator<double, double></code>
  to build a completely double-precision floating point correlator. For other input data try other compinations,
  like <code>MultiTauCorrelator<uint16_t, uint32_t></code>. But note that the resultant correlation function
  will always be a double array, as it has to be able to represent floating point values!

  \tparam C data type used for the shift registers.
  \tparam Cmonitor data type used for the monitor channels, i.e. for summing up the input values in each linear correlator.

  \subsection MultiTauCorrelator_Operation Operation
  This class has two modes of operation:
   -# you can supply a complete timeseries with N items that will be correlated in one call of correlate_series()
   -# You can supply the single values of the timeseries one after the other, i.e. do an on-line correlation
      by calling correlate_step() for each element.
  .

  When all values have been supplied, you can call normalize() to do the normalization of the correlation
  function. getCor() and getCorTau() return pointers to arrays containing the lag-times and the correlation
  function. Each has getSlots() items.

  To generate <b>autocorrelation functions</b> you can use these methods:
    - correlate_series()
    - correlate_step()
    - normalize()
  .
  To generate <b>cross-correlation functions</b> you can use these methods:
    - crosscorrelate_step()
    - crossnormalize()
  .
  \attention DO NEVER MIX UP THESE TWO SETS OF FUNCTIONS!



 */
template <typename C, typename Cmonitor>
class MultiTauCorrelator {
    public:
        /** \brief the available normalization types for autocorrelation functions*/
        enum NormalizationType {
            AutoStandard,
            AutoSymmetrical
        };
        /** \brief the available normalization types crosscorrelation functions*/
        enum CrossNormalizationType {
            CrossStandard,
            CrossSymmetrical
        };
    private:
    protected:
        /** \brief used to estimate the memory consumption */
        double mem;
        /** \brief contains the normalized correlation function */
        double* corr;
        /** \brief contains the correlation function */
        double* corr_int;
        /** \brief contains the timesteps of the correlation function */
        double* corr_tau;
        /** \brief counter for the number of accumulations done in each correlator (needed for the normalization) */
        uint64_t* corr_N;
        /** \brief monitor channels, i.e. here we sum up the values correlated by each correlator.
         *
         * This is needed due to the finite number of values which are correlated, so for large lag-times it may occur
         * that this is smaller than for large lag-times (see papers on "symmetric normalization" for details)
         */
        Cmonitor* corr_sum;
        /** \brief undelayed monitor for the second input signal for cross-correlation */
        Cmonitor corr_sum2;
        /** \brief number of linear correlators (i.e. number of decades) */
        unsigned int S;
        /** \brief binning ratio between two subsequent linear correlators */
        unsigned int m;
        /** \brief items in each correlator */
        unsigned int P;
        /** \brief smalles timestep, i.e. timestep in the first correlator */
        double tau_min;

        /** \brief slots in the correlator */
        unsigned int corr_slots;

        /** \brief the \f$ \Delta t_s \f$ of the s-th correlator */
        double* tau;

        /** \brief zero-delay channels \f$ n^{(i)}_1(t) \f$ of the linear correlators, signal 1 */
        C* tau0channel_signal1;

        /** \brief zero-delay channels \f$ n^{(i)}_1(t) \f$ of the linear correlators, signal 2 */
        C* tau0channel_signal2;

        /** \brief shift registers for the single correlators as pseudo 2D-array */
        std::vector<ShiftRegister<C> > shift_reg;

        /** \brief initialises the correlator datastructure */
        inline void init(){
            clear();
            mem=0;

            tau=(double*)ShiftRegister__calloc(S, sizeof(double));
            mem+=S*sizeof(double);
            tau0channel_signal1=(C*)ShiftRegister__calloc(S+1, sizeof(C));
            tau0channel_signal2=(C*)ShiftRegister__calloc(S+1, sizeof(C));
            mem+=(S+1)*2*sizeof(C);
            shift_reg=std::vector<ShiftRegister<C> >(S, ShiftRegister<C>(P));
            mem+=P*S*sizeof(C);
            tau[0]=tau_min;
            corr_slots=P+(S-1)*(P-P/m);
            for (unsigned int s=1; s<S; s++) {
                tau[s]=tau[s-1]*(double)m;
            }
            corr_int=(double*)ShiftRegister__calloc(corr_slots, sizeof(double));
            corr=(double*)ShiftRegister__calloc(corr_slots, sizeof(double));
            mem+=corr_slots*2*sizeof(double);
            corr_N=(uint64_t*)ShiftRegister__calloc(S, sizeof(uint64_t));
            mem+=S*sizeof(uint64_t);
            corr_sum=(Cmonitor*)ShiftRegister__calloc(corr_slots, sizeof(Cmonitor));
            corr_sum2=0;
            mem+=corr_slots*sizeof(Cmonitor);

            corr_tau=(double*)ShiftRegister__calloc(corr_slots, sizeof(double));
            mem+=(corr_slots)*sizeof(double);
            corr_tau[0]=tau_min;
            for (unsigned int i=1; i<P; i++) {
                corr_tau[i]=corr_tau[i-1]+tau[0];
                corr_int[i]=0;
                corr[i]=1;
            }
            int k=P;
            for (unsigned int s=1; s<S; s++) {
                for (unsigned int i=0; i<P-P/m; i++) {
                    corr_tau[k]=corr_tau[k-1]+tau[s];
                    corr_int[k]=0;
                    corr[k]=1;
                    k++;
                }
            }
        };

        /** \brief clear all reserved memory */
        inline void clear() {
            if (corr_int!=NULL) ShiftRegister__free(corr_int); corr_int=NULL;
            if (corr!=NULL) ShiftRegister__free(corr); corr=NULL;
            if (corr_tau!=NULL) ShiftRegister__free(corr_tau); corr_tau=NULL;
            if (tau!=NULL) ShiftRegister__free(tau); tau=NULL;
            if (tau0channel_signal1!=NULL) ShiftRegister__free(tau0channel_signal1); tau0channel_signal1=NULL;
            if (tau0channel_signal2!=NULL) ShiftRegister__free(tau0channel_signal2); tau0channel_signal2=NULL;
            if (corr_N!=NULL) ShiftRegister__free(corr_N); corr_N=NULL;
            if (corr_sum!=NULL) ShiftRegister__free(corr_sum); corr_sum=NULL;
            corr_sum2=0;
        };

    public:
        /** \brief class constructor */
       explicit inline  MultiTauCorrelator(unsigned int S=5, unsigned int m=3, unsigned int P=9, double tau_min=1e-9){
            this->S=S;
            this->m=m;
            this->P=P;
            this->tau_min=tau_min;
            corr_int=NULL;
            corr=NULL;
            corr_tau=NULL;
            corr_N=NULL;
            corr_sum=NULL;
            corr_sum2=0;
            tau=NULL;
            tau0channel_signal1=NULL;
            tau0channel_signal2=NULL;
            init();
        };

        /** \brief class destructor */
        inline ~MultiTauCorrelator() {
            if (corr_int!=NULL) ShiftRegister__free(corr_int); corr_int=NULL;
            if (corr!=NULL) ShiftRegister__free(corr); corr=NULL;
            if (corr_tau!=NULL) ShiftRegister__free(corr_tau); corr_tau=NULL;
            if (corr_N!=NULL) ShiftRegister__free(corr_N); corr_N=NULL;
            if (corr_sum!=NULL) ShiftRegister__free(corr_sum); corr_sum=NULL;
            corr_sum2=0;
            if (tau!=NULL) ShiftRegister__free(tau); tau=NULL;
            if (tau0channel_signal1!=NULL) ShiftRegister__free(tau0channel_signal1); tau0channel_signal1=NULL;
            if (tau0channel_signal2!=NULL) ShiftRegister__free(tau0channel_signal2); tau0channel_signal2=NULL;
            shift_reg.clear();
        };

        /** \brief returns the lag times of the correlation function  */
        inline double* getCorTau() { return corr_tau; };

        /** \brief returns the values normalized correlation function
         *
         * \note this only returns valid values after a call to normalize() or crossnormalize().
         */
        inline double* getCor() { return corr; };

        /** \brief returns the values of the non-normalized correlation function  */
        inline double* getCorNonNormalized() { return corr_int; };

        /** \brief returns the number of slots in the correlator (i.e. the number of items in \a corr_int and \a corr_tau ) */
        inline unsigned int getSlots() { return corr_slots; };



        /** \brief add another value to the auto-correlation function */
        inline void correlate_step(C value) {
            // go through all correlators
            //   if we have to shift one, do so, update the result array and reset its zero-lag-register tau0channel_signal1
            //   otherwise increase its tau0channel_signal1
            register C ts=value;
            tau0channel_signal1[0]=ts;
            shift_reg[0].multiplyAddToExternalDouble(corr_int, ts);
            shift_reg[0].addToExternal(corr_sum);
            shift_reg[0].push_right(ts);
            corr_N[0]++;
            //std::cout<<"shifting "<<0<<" @ i="<<i<<"   startchannel="<<0<<"   count="<<P<<"   tau0channel_signal1="<<tau0channel_signal1[0]<<" "<<doublevectortostr(tau0channel_signal1, S);//<<std::endl;
            unsigned long start=P/m;
            unsigned long count=P-P/m;
            //std::cout<<"   sr(0) = "<<shift_reg[0].to_string()<<std::endl;
            unsigned int ip=m;
            for (register unsigned int s=1; s<S; s++) {
                tau0channel_signal2[s]=tau0channel_signal1[s]=tau0channel_signal1[s]+ts;
                if (corr_N[0]%ip==0) {
                    register C fac=tau0channel_signal1[s];
                    //std::cout<<"shifting "<<s<<" @ i="<<i<<"   startchannel="<<P+(s-1)*(P-P/m)<<"   count="<<count<<"   tau0channel_signal1="<<tau0channel_signal1[s]<<" "<<doublevectortostr(tau0channel_signal1, S);//<<std::endl;
                    corr_N[s]++;
                    //std::cout<<"corr_N["<<s<<"]="<<corr_N[s]<<"   fac="<<fac<<"   corr_sum["<<s<<"]="<<corr_sum[s]<<std::endl;
                    //std::cout<<"   sr("<<s<<") = "<<shift_reg[s].to_string()<<std::endl;
                    //std::cout<<"  s="<<s<<"   P+(s-1)*(P-P/m)="<<P+(s-1)*(P-P/m)<<std::endl;
                    shift_reg[s].multiplyAddToExternal(&(corr_int[P+(s-1)*(P-P/m)]), fac, start, count);
                    shift_reg[s].addToExternal(&(corr_sum[P+(s-1)*(P-P/m)]), start, count);

                    // sum up the shift-out in the next 0-lag-register
                    //std::cout<<std::endl<<"   fac="<<fac<<"   tau0channel_signal1[s]="<<tau0channel_signal1[s]<<std::endl;
                    shift_reg[s].push_right(fac);

                    tau0channel_signal2[s]=tau0channel_signal1[s]=0; // reset tau0channel_signal1[s]
                }
                ip=ip*m;
            }
        }


        /** \brief calculates the correlation function */
        inline void correlate_series(C* timeseries, unsigned long long timeseries_N) {
            for (register unsigned long long i=1; i<=timeseries_N; i++) {
                correlate_step(timeseries[i-1]);
            }
        };

        /** \brief normalize the result of autocorrelation
         *
         * \note use crossnormalize() to normalize cross correlation functions
         */
        inline void normalize(MultiTauCorrelator::NormalizationType normalization=MultiTauCorrelator::AutoSymmetrical) {
            // normalize
            register int current_s=0;
            register unsigned int next_slots=P;
            register double N,N0,nk,n0;
            N=(double)corr_N[current_s];
            N0=(double)corr_N[0];
            nk=(double)corr_sum[current_s];
            n0=(double)corr_sum[0];
            register int slot_in_block=0;
            //std::cout<<current_s+1<<"/"<<S<<"     corr_N[current_s]="<<corr_N[current_s]<<"     corr_sum[current_s]="<<corr_sum[current_s]<<std::endl;
            //std::cout<<current_s+1<<"/"<<S<<"     corr_N[current_s]="<<corr_N[current_s]<<"     corr_sum[current_s]="<<corr_sum[current_s]<<"     corr_sum_norm[current_s]="<<corr_sum[current_s]/corr_N[current_s]<<"     n[current_s]="<<nk<<std::endl;
            //std::cout<<current_s<<"p="<<p<<"\n";
            for (register unsigned int i=0; i<corr_slots; i++) {
                if (i==next_slots) {
                    current_s++;
                    next_slots=next_slots+P-P/m;
                    N=(double)corr_N[current_s];
                    slot_in_block=0;
                    //std::cout<<current_s+1<<"/"<<S<<"     corr_N[current_s]="<<corr_N[current_s]<<"     corr_N_corected[current_s]="<<corr_N[current_s]*pow(2, current_s)<<"     corr_sum[current_s]="<<corr_sum[current_s]<<"     corr_sum_norm[current_s]="<<corr_sum[current_s]/pow(2, current_s)/corr_N[current_s]<<std::endl;
                }
                if ((corr_int[i]==0)|(corr_N[current_s]-slot_in_block<=0)) {
                    corr[i]=1.0;
                } else {
                    // see "A distributed algorithm for multi-tau autocorrelation"
                    nk=(double)corr_sum[i];
                    if (normalization==MultiTauCorrelator::AutoSymmetrical) corr[i]=N0*(double)corr_int[i]/pow(2.0, current_s)/n0/nk; //corr[i]=(double)corr_int[i]/((N-(double)slot_in_block)*pow(2, current_s))/norm_nk/norm_n0;
                    else corr[i]=N*(double)corr_int[i]/n0/n0;
                    if (std::isnan(corr[i])||std::isinf(corr[i])) corr[i]=1.0;
                }
                slot_in_block++;
                //corr_int[i]=corr_int[i]/(sum/(double)timeseries_N)/(corr_sum[current_s]/(double)timeseries_N);
            }
        }

        /** \brief add another value to the cross-correlation function */
        inline void crosscorrelate_step(C value1, C value2) {
            // go through all correlators
            //   if we have to shift one, do so, update the result array and reset its zero-lag-register tau0channel_signal1
            //   otherwise increase its tau0channel_signal1
            register C ts1=value1;
            register C ts2=value2;
            corr_sum[0]=corr_sum[0]+ts1;
            corr_sum2=corr_sum2+ts2;
            tau0channel_signal1[0]=ts1;
            tau0channel_signal2[0]=ts2;
            shift_reg[0].multiplyAddToExternalDouble(corr_int, ts2);
            shift_reg[0].addToExternal(corr_sum);
            shift_reg[0].push_right(ts1);
            corr_N[0]++;
            //std::cout<<"shifting "<<0<<" @ i="<<i<<"   startchannel="<<0<<"   count="<<P<<"   tau0channel_signal1="<<tau0channel_signal1[0]<<" "<<doublevectortostr(tau0channel_signal1, S);//<<std::endl;
            //unsigned long startC=P;
            unsigned long start=P/m;
            unsigned long count=P-P/m;
            //std::cout<<"   sr(0) = "<<shift_reg[0].to_string()<<std::endl;
            unsigned int ip=m;
            for (register unsigned int s=1; s<S; s++) {
                tau0channel_signal1[s]=tau0channel_signal1[s]+ts1;
                tau0channel_signal2[s]=tau0channel_signal2[s]+ts2;
                if (corr_N[0]%ip==0) {
                    const C fac1=tau0channel_signal1[s];
                    const C fac2=tau0channel_signal2[s];
                    //std::cout<<"shifting "<<s<<" @ i="<<i<<"   startchannel="<<P+(s-1)*(P-P/m)<<"   count="<<count<<"   tau0channel_signal1="<<tau0channel_signal1[s]<<" "<<doublevectortostr(tau0channel_signal1, S);//<<std::endl;
                    corr_N[s]++;
                    //std::cout<<"   sr("<<s<<") = "<<shift_reg[s].to_string()<<std::endl;
                    //std::cout<<"  s="<<s<<"   P+(s-1)*(P-P/m)="<<P+(s-1)*(P-P/m)<<std::endl;
                    shift_reg[s].multiplyAddToExternalDouble(&(corr_int[P+(s-1)*(P-P/m)]), fac2, start, count);
                    shift_reg[s].addToExternal(&(corr_sum[P+(s-1)*(P-P/m)]), start, count);

                    // sum up the shift-out in the next 0-lag-register
                    //std::cout<<std::endl<<"   fac="<<fac<<"   tau0channel_signal1[s]="<<tau0channel_signal1[s]<<std::endl;
                    shift_reg[s].push_right(fac1);

                    tau0channel_signal1[s]=0; // reset tau0channel_signal1[s]
                    tau0channel_signal2[s]=0; // reset tau0channel_signal1[s]
                }
                ip=ip*m;
            }
        }


        /** \brief calculates the correlation function */
        template <typename Cin>
        inline void crosscorrelate_series(Cin* timeseries1, Cin* timeseries2, unsigned long long timeseries_N) {
            for (register unsigned long long i=1; i<=timeseries_N; i++) {
                crosscorrelate_step(timeseries1[i-1], timeseries2[i-1]);
            }
        };


        /** \brief normalize the result of cross-correlation
         *
         * \note use normalize() to normalize auto-correlation functions
         */
        inline void crossnormalize(MultiTauCorrelator::CrossNormalizationType normalization=MultiTauCorrelator::CrossSymmetrical) {
            // normalize
            register int current_s=0;
            register unsigned int next_slots=P;
            register double N,nk;
            N=(double)corr_N[current_s];
            const double N0=(double)corr_N[0];
            nk=(double)corr_sum[current_s];
            const double n0=(double)corr_sum[0];
            const double n02=(double)corr_sum2;
            register int slot_in_block=0;
            //std::cout<<current_s+1<<"/"<<S<<"     corr_N[current_s]="<<corr_N[current_s]<<"     corr_sum[current_s]="<<corr_sum[current_s]<<std::endl;
            //std::cout<<current_s+1<<"/"<<S<<"     corr_N[current_s]="<<corr_N[current_s]<<"     corr_sum[current_s]="<<corr_sum[current_s]<<"     corr_sum_norm[current_s]="<<corr_sum[current_s]/corr_N[current_s]<<"     n[current_s]="<<nk<<std::endl;
            //std::cout<<current_s<<"p="<<p<<"\n";
            for (register unsigned int i=0; i<corr_slots; i++) {
                if (i==next_slots) {
                    current_s++;
                    next_slots=next_slots+P-P/m;
                    N=(double)corr_N[current_s];
                    slot_in_block=0;
                    //std::cout<<current_s+1<<"/"<<S<<"     corr_N[current_s]="<<corr_N[current_s]<<"     corr_N_corected[current_s]="<<corr_N[current_s]*pow(2, current_s)<<"     corr_sum[current_s]="<<corr_sum[current_s]<<"     corr_sum_norm[current_s]="<<corr_sum[current_s]/pow(2, current_s)/corr_N[current_s]<<std::endl;
                }
                if ((corr_int[i]==0)|(corr_N[current_s]-slot_in_block<=0)) {
                    corr[i]=1.0;
                } else {
                    // see "A distributed algorithm for multi-tau autocorrelation"
                    nk=(double)corr_sum[i];
                    if (normalization==MultiTauCorrelator::CrossSymmetrical) corr[i]=N0*(double)corr_int[i]/pow(2.0, current_s)/n02/nk; //corr[i]=(double)corr_int[i]/((N-(double)slot_in_block)*pow(2, current_s))/norm_nk/norm_n0;
                    else corr[i]=N*(double)corr_int[i]/n0/n02;
                    if (std::isnan(corr[i])||std::isinf(corr[i])) corr[i]=1.0;
                }
                slot_in_block++;
                //corr_int[i]=corr_int[i]/(sum/(double)timeseries_N)/(corr_sum[current_s]/(double)timeseries_N);
            }

        }


        /** \brief reset the correlator to begin a new correlation function */
        inline void reset() {
            corr_sum2=0;
            for (unsigned int s=0; s<S; s++) {
                tau0channel_signal1[s]=0;
                corr_sum[s]=0;
                corr_N[s]=0;
                shift_reg[s].clear();
            }
        }

        /** \brief returns the estimated memory consumption of the class in Bytes */
        inline double calc_mem_consumption() {
            return mem;
        };


};

#endif //MULTITAUCORRELATOR_H
