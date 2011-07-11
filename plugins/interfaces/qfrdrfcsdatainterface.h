#ifndef QFRDRFCSDATAINTERFACE_H
#define QFRDRFCSDATAINTERFACE_H

#include <QtPlugin>

/*! \brief represents a common data interface to access FCS correlation curves
    \ingroup qf3rdrdp_fcs

    This interfaces allows access to these features:
      - Each dataset may contain multiple (at least one!) measurements. Their number is returned by getCorrelationMeasurements().
        The interface is layed out in a way that the default parameters access the first measurement (0)!
      - Each measurement may contain a set of runs over which the program averages. The interface allows access to every
        single of these runs and also the average (and standard deviation) over all these runs. The number of runs is
        returned by getCorrelationRuns(). Note that only an average run has to be available, the singe runs are optional.
        However the number of runs in every measurement is equal!
      - every run (and also the average/stddev) contains the same number of data points \f$ (\tau_i, g_i) \f$ which is
        returned by getCorrelationN()
      - all runs (and also the average/stddev) have the same lag time axis. The lags are returned by getCorrelationT().
      - The data of the runs is returned by getCorrelationRun(), getCorrelationMean() and getCorrelationStdDev()
      - It is possible to select only a subset of runs. In that case isCorrelationRunVisible() returns \c true if a run is really visible
        and to be used.
      - Each measurement and each run may also carry a specific name which is returned by getCorrelationMeasurementsName() and
        getCorrelationRunName().
    .

 */
class QFRDRFCSDataInterface {
    public:
        virtual ~QFRDRFCSDataInterface() {}

        /** \brief number of correlation measurements */
        virtual int getCorrelationMeasurements()=0;
        /** \brief name of the given correlation measurements */
        virtual QString getCorrelationMeasurementsName(int measurement)=0;

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
         * access this as \code correlation[measurement*correlationN*runs + run*correlationN + n] \endcode
         */
        virtual double* getCorrelation()=0;
        /** \brief values of the correlation function for a specified run.
         *         This is a 1D array of size correlationN
         */
        virtual double* getCorrelationRun(int run, int measurement=0)=0;
        /** \brief errors, associated with the correlation function for a specified run.
         *         This is a 1D array of size correlationN
         */
        virtual double* getCorrelationRunError(int run, int measurement=0)=0;
        /** \brief values of the correlation function errors.
         *         This is a 2D array of size runs * correlationN
         *
         * access this as \code correlationRunErros[measurement*correlationN*runs + run*correlationN + n] \endcode
         */
        virtual double* getCorrelationRunErrors()=0;
        /** \brief return a string naming/describing a run (should be short) */
        virtual QString getCorrelationRunName(int run, int measurement=0)=0;
        /** \brief values of the averaged correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        virtual double* getCorrelationMean(int measurement=0)=0;
        /** \brief values of the standard deviation of the correlation function (averaged over all runs).
         *         This is a 1D array of size correlationN */
        virtual double* getCorrelationStdDev(int measurement=0)=0;

        /** \brief returns true when a given run is visible. the average run is indicated by -1 */
        virtual bool isCorrelationRunVisible(int run, int measurement=0)=0;

};

Q_DECLARE_INTERFACE( QFRDRFCSDataInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRFCSDataInterface/1.0")





#endif // QFRDRFCSDATAINTERFACE_H
