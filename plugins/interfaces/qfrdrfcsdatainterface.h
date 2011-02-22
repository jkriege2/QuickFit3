#ifndef QFRDRFCSDATAINTERFACE_H
#define QFRDRFCSDATAINTERFACE_H

/*! \brief represents a common data interface to access FCS correlation curves
    \ingroup qf3rdrdp_fcs

    This interfaces allows access to these features:
      - Each dataset may contain a set of runs over which the program averages. The interface allows access to every
        single of these runs and also the average (and standard deviation) over all these runs. The number of runs is
        returned by getCorrelationRuns(). Note that only an average run has to be available, the singe runs are optional.
      - every run (and also the average/stddev) contains the same number of data points \f$ (\tau_i, g_i) \f$ which is
        returned by getCorrelationN()
      - all runs (and also the average/stddev) have the same lag time axis. The lags are returned by getCorrelationT().
      - The data of the runs is returned by getCorrelationRun(), getCorrelationMean() and getCorrelationStdDev()
      - It is possible to select only a subset of runs. In that case runVisible() returns \c true if a run is really visible
        and to be used.
    .

 */
class QFRDRFCSDataInterface {
    public:
        virtual ~QFRDRFCSDataInterface() {}

        /** \brief number of correlation runs in this object */
        virtual int getCorrelationRuns()=0;
        /** \brief number of datapoints in every correlation curve */
        virtual long long getCorrelationN()=0;
        /** \brief sample points (times \f$ \tau \f$ ) of the correlation function.
         *         This is a 1D array of size correlationN */
        virtual double* getCorrelationT()=0;
        /** \brief values of the correlation function.
         *         This is a 2D array of size runs * correlationN
         *
         * access this as \code correlation[run*correlationN + n] \endcode
         */
        virtual double* getCorrelation()=0;
        /** \brief values of the correlation function for a specified run.
         *         This is a 1D array of size correlationN
         */
        virtual double* getCorrelationRun(int run)=0;
        /** \brief values of the averaged correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        virtual double* getCorrelationMean()=0;
        /** \brief values of the standard deviation of the correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        virtual double* getCorrelationStdDev()=0;

        /** \brief returns true when a given run is visible. the average run is indicated by -1 */
        virtual bool runVisible(int run)=0;

};

Q_DECLARE_INTERFACE( QFRDRFCSDataInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRFCSDataInterface/1.0")



//
//
//
//        /** \brief recalculate correlation curve mean and standard deviation */
//        virtual void recalculateCorrelations()=0;
//
//
//        /** \brief channel represented by this object (a file e.g. from ALV may contain several channels,
//         *         but only one channel is represented by one object. This is saved in the property
//         *         CHANNEL. */
//        virtual int getChannel()=0;
//        /** \brief number of countrate runs in this object */
//        virtual int getRateRuns()=0;
//        /** \brief number of datapoints in every count rate curve */
//        virtual long long getRateN()=0;
//        /** \brief sample points (times \f$ \tau \f$ ) of the count rate
//         *         This is a 1D array of size rateN */
//        virtual double* getRateT()=0;
//        /** \brief values of the count rate.
//         *         This is a 2D array of size runs * rateN
//         *
//         * access this as \code rate[run*rateN + n] \endcode
//         */
//        virtual double* getRate()=0;
//        /** \brief values of the count rate nfor a given run.
//         *         This is a 1D array of length  rateN
//         *
//         * access this as \code rate[run*rateN + n] \endcode
//         */
//        virtual double* getRateRun (int run)=0;
//
//
//        /** \brief number of binned count rate runs in this object */
//        virtual int getBinnedRateRuns()=0;
//        /** \brief number of datapoints in every binned count rate */
//        virtual long long getBinnedRateN()=0;
//        /** \brief sample points (times \f$ \tau \f$ ) of the binned count rate
//         *         This is a 1D array of size binnedRateN */
//        virtual double* getBinnedRateT()=0;
//        /** \brief values of the binned count rate.
//         *         This is a 2D array of size runs * binnedRateN
//         *
//         * access this as \code rate[run*binnedRateN + n] \endcode
//         */
//        virtual double* getBinnedRate()=0;
//        /** \brief values of the binned count rate for a given run.
//         *         This is a 1D array of length  binnedRateN
//         *
//         * access this as \code rate[run*binnedRateN + n] \endcode
//         */
//        virtual double* getBinnedRateRun (int run)=0;
//
//
//        /** \brief calculate the mean value of the count rate */
//        virtual double calcRateMean(int run=0)=0;
//        /** \brief calculate the standard deviation of the count rate */
//        virtual double calcRateStdDev(int run=0)=0;
//        /** \brief calculate minimum and maximum count rates */
//        virtual void calcRateMinMax(int run, double& min, double& max)=0;
//
//        /** \brief set the number of datapoints in every binned count rate run.
//         *
//         * \note this function does only take effect BEFORE loading a dataset!
//         *       If you want to recalculate the binned count rate call calcBinnedRate() afterwards!
//         */
//        virtual void setBinnedRateN(int value)=0;
//
//        /** \brief this function changes the size of the binned count rate and recalculates it
//         *         depending on the value of autoCalcRateN. */
//        virtual void calcBinnedRate()=0;
//
//        /** \brief returns whether to leave out a run */
//        virtual bool leaveoutRun(int run)=0;
//        /** \brief add a run to the leaveouts */
//        virtual void leaveoutAddRun(int run)=0;
//        /** \brief remove a run from the leaveouts */
//        virtual void leaveoutRemoveRun(int run)=0;
//        /** \brief clear all leaveouts */
//        virtual void leaveoutClear()=0;
//        /** \brief set whether the given run is visible. the average run is indicated by -1 */
//        virtual void setRunVisible(int run, bool vis)=0;



#endif // QFRDRFCSDATAINTERFACE_H
