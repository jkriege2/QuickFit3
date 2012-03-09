#ifndef QFRDRCOUNTRATESINTERFACE_H
#define QFRDRCOUNTRATESINTERFACE_H

#include <QtPlugin>

/*! \brief interface for a set of countrates
    \ingroup qf3rdrdp_fcs

  Every count rates dataset contains these elements:
    - a set of count rate curves plus a second set with the count rates in reduced resolution.
      This second set may be used for quick plotting of the count rates, as for long measurements
      with high timeresolution plotting may otherwise be quite tedious. The property autoCalcRateN
      contains <=0 if this binned count rate should NOT be calculated by the object, but is loaded i.e.
      from a file and otherwise the desired number of count rate datapoints in the binned count rate.
      The binned countrate is calculated only if autoCalcRateN<rateN!
  .

  It is also possible to define a leaveout list which is a list of the runs that shall not be taken into
  account for the correlation average. This may be usefull, if only some few of the runs in a file are damaged
  (e.g. by agregates moving through the focus ...).
*/
class QFRDRCountRatesInterface
{
    public:
        virtual ~QFRDRCountRatesInterface() {}

        /** \brief number of countrate runs in this object */
        virtual int getRateRuns()=0;
        /** \brief number of datapoints in every count rate curve */
        virtual long long getRateN()=0;
        /** \brief sample points (times \f$ \tau \f$ ) of the count rate
         *         This is a 1D array of size rateN */
        virtual double* getRateT()=0;
        /** \brief values of the count rate.
         *         This is a 2D array of size runs * rateN
         *
         * access this as \code rate[run*rateN + n] \endcode
         */
        virtual double* getRate()=0;
        /** \brief values of the count rate nfor a given run.
         *         This is a 1D array of length  rateN
         *
         * access this as \code rate[run*rateN + n] \endcode
         */
        virtual double* getRateRun (int run)=0;


        /** \brief number of binned count rate runs in this object */
        virtual int getBinnedRateRuns()=0;
        /** \brief number of datapoints in every binned count rate */
        virtual long long getBinnedRateN()=0;
        /** \brief sample points (times \f$ \tau \f$ ) of the binned count rate
         *         This is a 1D array of size binnedRateN */
        virtual double* getBinnedRateT()=0;
        /** \brief values of the binned count rate.
         *         This is a 2D array of size runs * binnedRateN
         *
         * access this as \code rate[run*binnedRateN + n] \endcode
         */
        virtual double* getBinnedRate()=0;
        /** \brief values of the binned count rate for a given run.
         *         This is a 1D array of length  binnedRateN
         *
         * access this as \code rate[run*binnedRateN + n] \endcode
         */
        virtual double* getBinnedRateRun (int run)=0;


        /** \brief calculate the mean value of the count rate */
        //virtual double calcRateMean(int run=0)=0;
        /** \brief calculate the standard deviation of the count rate */
        //virtual double calcRateStdDev(int run=0)=0;
        /** \brief return the mean value of the count rate, as last calculated by a call to calcRateMean() */
        virtual double getRateMean(int run=0)=0;
        /** \brief return the standard deviation of the count rate, as last calculated by a call to calcRateStdDev()  */
        virtual double getRateStdDev(int run=0)=0;
        /** \brief calculate minimum and maximum count rates */
        //virtual void calcRateMinMax(int run, double& min, double& max)=0;
        /** \brief calculate minimum and maximum count rates */
        virtual void getRateMinMax(int run, double& min, double& max)=0;

        /** \brief set the number of datapoints in every binned count rate run.
         *
         * \note this function does only take effect BEFORE loading a dataset!
         *       If you want to recalculate the binned count rate call calcBinnedRate() afterwards!
         */
        //virtual void setBinnedRateN(int value)=0;

        /** \brief this function changes the size of the binned count rate and recalculates it
         *         depending on the value of autoCalcRateN. */
        //virtual void calcBinnedRate()=0;
};

Q_DECLARE_INTERFACE( QFRDRCountRatesInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRCountRatesInterface/1.0")

#endif // QFRDRCOUNTRATESINTERFACE_H

