#ifndef QFRDRPHOTONCOUNTSINTERFACE_H
#define QFRDRPHOTONCOUNTSINTERFACE_H


#include <QtPlugin>

/*! \brief interface for photon count rates (from a set of channels)
    \ingroup qf3rdrdp_photoncounts

*/
class QFRDRPhotonCountsInterface
{
    public:
        virtual ~QFRDRPhotonCountsInterface() {}

        /** \brief name for a channel */
        virtual QString getPhotonCountsChannelName(int channel)=0;
        /** \brief number of countrate channels in this object */
        virtual int getPhotonCountsChannels()=0;
        /** \brief number of datapoints in every count rate curve */
        virtual long long getPhotonCountsN()=0;
        /** \brief sample points (times \f$ \tau \f$ ) of the count rate
         *         This is a 1D array of size rateN */
        virtual double* getPhotonCountsT()=0;
        /** \brief values of the count rate.
         *         This is a 2D array of size channels * rateN
         *
         * access this as \code rate[channel*rateN + n] \endcode
         */
        virtual double* getPhotonCounts()=0;
        /** \brief values of the count rate nfor a given channel.
         *         This is a 1D array of length  rateN
         *
         * access this as \code rate[channel*rateN + n] \endcode
         */
        virtual double* getPhotonCountsChannel (int channel)=0;


        /** \brief number of binned count rate channels in this object */
        virtual int getPhotonCountsBinnedChannels()=0;
        /** \brief number of datapoints in every binned count rate */
        virtual long long getPhotonCountsBinnedN()=0;
        /** \brief sample points (times \f$ \tau \f$ ) of the binned count rate
         *         This is a 1D array of size binnedRateN */
        virtual double* getPhotonCountsBinnedT()=0;
        /** \brief values of the binned count rate.
         *         This is a 2D array of size channels * binnedRateN
         *
         * access this as \code rate[channel*binnedRateN + n] \endcode
         */
        virtual double* getPhotonCountsBinned()=0;
        /** \brief values of the binned count rate for a given channel.
         *         This is a 1D array of length  binnedRateN
         *
         * access this as \code rate[channel*binnedRateN + n] \endcode
         */
        virtual double* getPhotonCountsBinnedChannel (int channel)=0;


        enum PhotonCountsUnits {
            KiloHertz=0,
            Counts=1
        };

        /** \brief return the mean value of the count rate, as last calculated by a call to calcRateMean()
         *
         *  The parameter \a unit tells the function in which units to output the value.
         */
        virtual double getPhotonCountsMean(int channel=0, PhotonCountsUnits unit=KiloHertz)=0;
        /** \brief return the standard deviation of the count rate, as last calculated by a call to calcRateStdDev()  */
        virtual double getPhotonCountsStdDev(int channel=0, PhotonCountsUnits unit=KiloHertz)=0;

        /** \brief calculate minimum and maximum count rates */
        virtual void getPhotonCountsMinMax(int channel, double& min, double& max, PhotonCountsUnits unit=KiloHertz)=0;

        /** \brief return a factor with which to multiply the data from getPhotonCounts() to achieve a certain unit */
        virtual double getPhotonCountsUnit(PhotonCountsUnits unit=KiloHertz)=0;
        /** \brief return a factor with which to multiply the data from getPhotonCountsBinnes() to achieve a certain unit */
        virtual double getPhotonCountsBinnedUnit(PhotonCountsUnits unit=KiloHertz)=0;
};

Q_DECLARE_INTERFACE( QFRDRPhotonCountsInterface,
                     "www.dkfz.de.b040.quickfit3.fcsplugin.QFRDRPhotonCountsInterface/1.0")


#endif // QFRDRPHOTONCOUNTSINTERFACE_H
