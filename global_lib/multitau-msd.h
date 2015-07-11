#ifndef MULTITAUMSD_H
#define MULTITAUMSD_H

#include "shiftregister.h"
#include <vector>
#include <stdint.h>
#include <cstdlib>
#include <iostream>
#include <cmath>

/** \file multitau-msd.h
 *  \ingroup tools_math_stat
 */

/*! \brief returns the mean squared displacement (MSD) of a given timeseries x(t) calculated on-line
    \ingroup tools_math_stat

    This class calculates an estimate of the mean squared displacement (MSD) from a time series of positions
      \f[ \langle\hat{r}^2(\tau)\rangle=\frac{1}{N-\tau}\cdot\sum\limits_{i=\tau}^{N}(X_i-X_{i-\tau})^2 \f]
    To do so it uses a multi-tau scheme and the MSD is calculated on-line, i.e. the complete trajectory x(t) does not
    have to be completely present in memory. This code is adapted from the multi-tau correlator scheme implemented
    in multitau-correlator.h and also uses the ShiftRegister class for it's implementation.

    The algorithm works like this: It creates S shift registers of length P. The trajectory steps x(i) are pushed into
    the first register, so the first entry contains the current position, the second, the last position, the third entry
    the second-last position ... Then the distance of the current position to every one of these old positions is calculated
    and summed up, so we get \f$ \sum_i (x(i)-x(i-1))^2 \f$ from the first register entry, \f$ \sum_i (x(i)-x(i-2))^2 \f$
    from the next ... When the values fal out of the first register after P steps, every m-th value is pushed into the next
    register. The other m-1 values are simply dropped (for long trajectories this will not create a too large error). The
    second register is treated like the first, except that the calculations are only performed every m-th step, so this register
    calculates \f$ \sum_i (x(i)-x(i-P))^2; \sum_i (x(i)-x(i-P-m))^2; \sum_i (x(i)-x(i-P-2m))^2; ...  \f$. This is then
    repeated recursivley for all S registers and thus the MSD is sampled at semi-logarithmically spaced lag times.

    After pushing in all steps using contribute_step() or contribute_series(), you will have to normalize the result using
    the method normalize() and can finally read the result using getMSD() and getMSDTau(). The method normalize() will divide
    the sum for each lag time tau by the number of summands.

    This class is able to process 1D, 2D and 3D timeseries, but never mix them in one instance. There are no internal mechanisms
    to prevent this mixing and it will lead to false results.


 */
template <class C>
class MultiTauMSD {
    private:
    protected:
        /** \brief used to estimate the memory consumption */
        double mem;
        /** \brief contains the normalized msd function */
        double* msd;
        /** \brief contains the msd function */
        double* msd_int;
        /** \brief contains the timesteps of the msd function */
        double* msd_tau;
        /** \brief counter for the number of accumulations done in each msd (needed for the normalization) */
        uint64_t* msd_N;
        /** \brief number of shift registers (i.e. number of decades) */
        unsigned int S;
        /** \brief binning ratio between two subsequent shift registers */
        unsigned int m;
        /** \brief items in each shift register */
        unsigned int P;
        /** \brief smalles timestep, i.e. timestep in the first shift register */
        double tau_min;

        /** \brief slots in the msd calculation object */
        unsigned int msd_slots;

        /** \brief the \f$ \Delta t_s \f$ of the s-th shift register */
        double* tau;


        /** \brief shift registers  */
        std::vector<ShiftRegister<C> > shift_reg;
        /** \brief shift registers for y-coordinate */
        std::vector<ShiftRegister<C> > shift_regy;
        /** \brief shift registers for z-coordinate */
        std::vector<ShiftRegister<C> > shift_regz;

        /** \brief initialises the msd datastructure */
        inline void init(){
            clear();
            mem=0;

            tau=(double*)calloc(S, sizeof(double));
            mem+=S*sizeof(double);
            shift_reg=std::vector<ShiftRegister<C> >(S, ShiftRegister<C>(P));
            shift_regy=std::vector<ShiftRegister<C> >(S, ShiftRegister<C>(P));
            shift_regz=std::vector<ShiftRegister<C> >(S, ShiftRegister<C>(P));
            mem+=P*S*sizeof(C)*3;
            tau[0]=tau_min;
            msd_slots=P+(S-1)*(P-P/m);
            for (unsigned int s=1; s<S; s++) {
                tau[s]=tau[s-1]*(double)m;
            }
            msd_int=(double*)calloc(msd_slots, sizeof(double));
            msd=(double*)calloc(msd_slots, sizeof(double));
            mem+=msd_slots*2*sizeof(double);
            msd_N=(uint64_t*)calloc(S, sizeof(uint64_t));
            mem+=S*sizeof(uint64_t);

            msd_tau=(double*)calloc(msd_slots, sizeof(double));
            mem+=(msd_slots)*sizeof(double);
            msd_tau[0]=tau_min;
            for (unsigned int i=1; i<P; i++) {
                msd_tau[i]=msd_tau[i-1]+tau[0];
                msd_int[i]=0;
                msd[i]=1;
            }
            int k=P;
            for (unsigned int s=1; s<S; s++) {
                for (unsigned int i=0; i<P-P/m; i++) {
                    msd_tau[k]=msd_tau[k-1]+tau[s];
                    msd_int[k]=0;
                    msd[k]=1;
                    k++;
                }
            }        };

        /** \brief clear all reserved memory */
        inline void clear() {
            if (msd_int!=NULL) free(msd_int); msd_int=NULL;
            if (msd!=NULL) free(msd); msd=NULL;
            if (msd_tau!=NULL) free(msd_tau); msd_tau=NULL;
            if (tau!=NULL) free(tau); tau=NULL;
            if (msd_N!=NULL) free(msd_N); msd_N=NULL;
        };

    public:
        /** \brief class constructor */
        MultiTauMSD(unsigned int S=5, unsigned int m=3, unsigned int P=9, double tau_min=1e-9){
            this->S=S;
            this->m=m;
            this->P=P;
            this->tau_min=tau_min;
            msd_int=NULL;
            msd=NULL;
            msd_tau=NULL;
            msd_N=NULL;
            tau=NULL;
            init();
        };

        /** \brief class destructor */
        ~MultiTauMSD() {
            if (msd_int!=NULL) free(msd_int); msd_int=NULL;
            if (msd!=NULL) free(msd); msd=NULL;
            if (msd_tau!=NULL) free(msd_tau); msd_tau=NULL;
            if (msd_N!=NULL) free(msd_N); msd_N=NULL;
            if (tau!=NULL) free(tau); tau=NULL;
            shift_reg.clear();
            shift_regy.clear();
            shift_regz.clear();
        };

        /** \brief returns the lag times of the msd function  */
        inline double* getMSDTau() { return msd_tau; };

        /** \brief returns the values normalized msd function
         *
         * \note this only returns valid values after a call to normalize().
         */
        inline double* getMSD() { return msd; };

        /** \brief returns the values of the non-normalized msd function  */
        inline double* getMSDNonNormalized() { return msd_int; };

        /** \brief returns the number of slots in the msd estimator (i.e. the number of items in \a msd_int and \a msd_tau ) */
        inline unsigned int getSlots() { return msd_slots; };



        /** \brief add another 3D-value to the msd function */
        inline void contribute_step(C vx, C vy, C vz) {
            register C ts=vx;
            register C tsy=vy;
            register C tsz=vz;
            shift_reg[0].squareDistanceAddToExternal(msd_int, ts);
            shift_reg[0].push_right(ts);
            shift_regy[0].squareDistanceAddToExternal(msd_int, tsy);
            shift_regy[0].push_right(tsy);
            shift_regz[0].squareDistanceAddToExternal(msd_int, tsz);
            shift_regz[0].push_right(tsz);
            msd_N[0]++;
            unsigned long start=P/m;
            unsigned long count=P-P/m;
            unsigned int ip=m;
            for (register unsigned int s=1; s<S; s++) {
                if (msd_N[0]%ip==0) {
                    register C fac=ts;
                    register C facy=tsy;
                    register C facz=tsz;
                    msd_N[s]++;
                    shift_reg[s].squareDistanceAddToExternal(&(msd_int[P+(s-1)*(P-P/m)]), fac, start, count);
                    shift_reg[s].push_right(fac);
                    shift_regy[s].squareDistanceAddToExternal(&(msd_int[P+(s-1)*(P-P/m)]), facy, start, count);
                    shift_regy[s].push_right(facy);
                    shift_regz[s].squareDistanceAddToExternal(&(msd_int[P+(s-1)*(P-P/m)]), facz, start, count);
                    shift_regz[s].push_right(facz);

                }
                ip=ip*m;
            }
        }




        /** \brief add another 2D-value to the msd function */
        inline void contribute_step(C vx, C vy) {
            register C ts=vx;
            register C tsy=vy;
            shift_reg[0].squareDistanceAddToExternal(msd_int, ts);
            shift_reg[0].push_right(ts);
            shift_regy[0].squareDistanceAddToExternal(msd_int, tsy);
            shift_regy[0].push_right(tsy);
            msd_N[0]++;
            unsigned long start=P/m;
            unsigned long count=P-P/m;
            unsigned int ip=m;
            for (register unsigned int s=1; s<S; s++) {
                if (msd_N[0]%ip==0) {
                    register C fac=ts;
                    register C facy=tsy;
                    msd_N[s]++;
                    shift_reg[s].squareDistanceAddToExternal(&(msd_int[P+(s-1)*(P-P/m)]), fac, start, count);
                    shift_reg[s].push_right(fac);
                    shift_regy[s].squareDistanceAddToExternal(&(msd_int[P+(s-1)*(P-P/m)]), facy, start, count);
                    shift_regy[s].push_right(facy);

                }
                ip=ip*m;
            }
        }


        /** \brief add another value to the msd function */
        inline void contribute_step(C value) {
            register C ts=value;
            shift_reg[0].squareDistanceAddToExternal(msd_int, ts);
            shift_reg[0].push_right(ts);
            msd_N[0]++;
            unsigned long start=P/m;
            unsigned long count=P-P/m;
            unsigned int ip=m;
            for (register unsigned int s=1; s<S; s++) {
                if (msd_N[0]%ip==0) {
                    register C fac=ts;
                    msd_N[s]++;
                    shift_reg[s].squareDistanceAddToExternal(&(msd_int[P+(s-1)*(P-P/m)]), fac, start, count);

                    shift_reg[s].push_right(fac);

                }
                ip=ip*m;
            }
        }


        /** \brief calculates the msd function */
        void contribute_series(C* timeseries, unsigned long long timeseries_N) {
            for (register unsigned long long i=1; i<=timeseries_N; i++) {
                contribute_step(timeseries[i-1]);
            }
        };



        /** \brief reset the msd calculator to begin a new msd function */
        void reset() {
            for (unsigned int s=0; s<S; s++) {
                msd_N[s]=0;
                shift_reg[s].clear();
                shift_regy[s].clear();
                shift_regz[s].clear();
            }
        }

        /** \brief returns the estimated memory consumption of the class in Bytes */
        double calc_mem_consumption() {
            return mem;
        };

        /** \brief normalize the msd
         */
        void normalize() {
            // normalize
            register int current_s=0;
            register unsigned int next_slots=P;
            register double N;
            N=(double)msd_N[current_s];
            register int slot_in_block=0;
            for (register unsigned int i=0; i<msd_slots; i++) {
                if (i==next_slots) {
                    current_s++;
                    next_slots=next_slots+P-P/m;
                    N=(double)msd_N[current_s];
                    slot_in_block=0;
                }
                if ((msd_int[i]==0)|(msd_N[current_s]-slot_in_block<=0)) {
                    msd[i]=0.0;
                } else {
                    msd[i]=(double)msd_int[i]/N;
                    if (std::isnan(msd[i])||std::isinf(msd[i])) msd[i]=0.0;
                }
                slot_in_block++;
            }
        }

};

#endif //MULTITAUMSD_H
